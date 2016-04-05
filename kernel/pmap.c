#include"include/memlayout.h"
#include"include/x86.h"
#include"include/pmap.h"
#include"include/string.h"

enum {
	// Kernel error codes -- keep in sync with list in lib/printfmt.c.
	E_UNSPECIFIED	= 1,	// Unspecified or unknown problem
	E_BAD_ENV	= 2,	// Environment doesn't exist or otherwise
				// cannot be used in requested action
	E_INVAL		= 3,	// Invalid parameter
	E_NO_MEM	= 4,	// Request failed due to memory shortage
	E_NO_FREE_ENV	= 5,	// Attempt to create a new environment beyond
				// the maximum allowed
	E_FAULT		= 6,	// Memory fault
	E_NO_SYS	= 7,	// Unimplemented system call

	MAXERROR
};
//#define npages (1<<15)			// Amount of physical memory (in pages)

// These variables are set in mem_init()
//pde_t *kern_pgdir;		// Kernel's initial page directory
struct PageInfo pages[npages];		// Physical page state array
static struct PageInfo *page_free_list;	// Free list of physical pages

bool empty_list(struct PageInfo*pt){
	if(!pt){
		return true;
	}
	else 
		return false;
}
// --------------------------------------------------------------
// Tracking of physical pages.
// The 'pages' array has one 'struct PageInfo' entry per physical page.
// Pages are reference counted, and free pages are kept on a linked list.
// --------------------------------------------------------------

void
page_init(void)
{
	// The example code here marks all physical pages as free.
	// However this is not truly the case.  What memory is free?
	// 
	// NB: DO NOT actually touch the physical memory corresponding to
	// free pages!
	unsigned long i;
	for (i = 0; i < npages; i++) {
		pages[i].pp_ref = 0;
		pages[i].pp_link = page_free_list;
		page_free_list = &pages[i];
	}
}

//
// Allocates a physical page.  If (alloc_flags & ALLOC_ZERO), fills the entire
// returned physical page with '\0' bytes.  Does NOT increment the reference
// count of the page - the caller must do these if necessary (either explicitly
// or via page_insert).
//
// Be sure to set the pp_link field of the allocated page to NULL so
// page_free can check for double-free bugs.
//
// Returns NULL if out of free memory.
//
// Hint: use page2kva and memset
struct PageInfo *
page_alloc(int alloc_flags)
{
	// Fill this function in
	if(page_free_list==NULL){
		return NULL;
	}
	
	struct PageInfo* pt=page_free_list;
	page_free_list=page_free_list->pp_link;  
	if(alloc_flags&ALLOC_ZERO)
		memset(page2kva(pt),0,PGSIZE);	
	return pt;
	
}

//
// Return a page to the free list.
// (This function should only be called when pp->pp_ref reaches 0.)
//
void
page_free(struct PageInfo *pp)
{
	// Fill this function in
	// Hint: You may want to panic if pp->pp_ref is nonzero or
	// pp->pp_link is not NULL.
	/*if((pp->pp_ref!=0)||(pp->pp_link!=NULL)){
		panic("error");
		return;
	}*/
	//else{
		pp->pp_link=page_free_list;
		page_free_list=pp;
		return ;
	//}


}

//
// Decrement the reference count on a page,
// freeing it if there are no more refs.
//
void
page_decref(struct PageInfo* pp)
{
	if (--pp->pp_ref == 0)
		page_free(pp);
}

// Given 'pgdir', a pointer to a page directory, pgdir_walk returns
// a pointer to the page table entry (PTE) for linear address 'va'.
// This requires walking the two-level page table structure.
//
// The relevant page table page might not exist yet.
// If this is true, and create == false, then pgdir_walk returns NULL.
// Otherwise, pgdir_walk allocates a new page table page with page_alloc.
//    - If the allocation fails, pgdir_walk returns NULL.
//    - Otherwise, the new page's reference count is incremented,
//	the page is cleared,
//	and pgdir_walk returns a pointer into the new page table page.
//
// Hint 1: you can turn a Page * into the physical address of the
// page it refers to with page2pa() from pmap.h.
//
// Hint 2: the x86 MMU checks permission bits in both the page directory
// and the page table, so it's safe to leave permissions in the page
// directory more permissive than strictly necessary.
//
// Hint 3: look at inc/mmu.h for useful macros that mainipulate page
// table and page directory entries.
//
pte_t *
pgdir_walk(pde_t *pgdir, const void *va, int create)
{
	// Fill this function in
	struct PageInfo *newpage;
	physaddr_t pa;
	pte_t *result;
	if(!pgdir[PDX(va)]){
		if(create==false){
			return NULL;
		}
		else if(!(newpage=page_alloc(1))){
			return NULL;
		}
		else{
			newpage->pp_ref++;
			pa=page2pa(newpage);
			memset((void*)pa,0,PGSIZE);
			//alloc a page to store a page table
			pgdir[PDX(va)]=pa|PTE_P|PTE_W|PTE_U;
			result=page2kva(newpage);
		}
	}
	else{
		//kva result is the addr of page table
		result=page2kva(pa2page(pgdir[PDX(va)]));
	}
	return &result[PTX(va)];
}

//
// Map [va, va+size) of virtual address space to physical [pa, pa+size)
// in the page table rooted at pgdir.  Size is a multiple of PGSIZE, and
// va and pa are both page-aligned.
// Use permission bits perm|PTE_P for the entries.
//
// This function is only intended to set up the ``static'' mappings
// above UTOP. As such, it should *not* change the pp_ref field on the
// mapped pages.
//
// Hint: the TA solution uses pgdir_walk
static void
boot_map_region(pde_t *pgdir, uintptr_t va, unsigned long size, physaddr_t pa, int perm)
{
	// Fill this function in
	pte_t *p;
	int i;
	for(i=0;i<size/PGSIZE;i++,va+=PGSIZE){
		p=pgdir_walk(pgdir,(void*)va,1);
		*p=(pa+PGSIZE*i)|perm|PTE_P;
	}
}

//
// Map the physical page 'pp' at virtual address 'va'.
// The permissions (the low 12 bits) of the page table entry
// should be set to 'perm|PTE_P'.
//
// Requirements
//   - If there is already a page mapped at 'va', it should be page_remove()d.
//   - If necessary, on demand, a page table should be allocated and inserted
//     into 'pgdir'.
//   - pp->pp_ref should be incremented if the insertion succeeds.
//   - The TLB must be invalidated if a page was formerly present at 'va'.
//
// Corner-case hint: Make sure to consider what happens when the same
// pp is re-inserted at the same virtual address in the same pgdir.
// However, try not to distinguish this case in your code, as this
// frequently leads to subtle bugs; there's an elegant way to handle
// everything in one code path.
//
// RETURNS:
//   0 on success
//   -E_NO_MEM, if page table couldn't be allocated
//
// Hint: The TA solution is implemented using pgdir_walk, page_remove,
// and page2pa.
//
int
page_insert(pde_t *pgdir, struct PageInfo *pp, void *va, int perm)
{
	// Fill this function in
	//pte_t *pte=pgdir_walk(pgdir,va,1);
	struct PageInfo* pg=page_lookup(pgdir,va,NULL);
	if(pg==pp){
		pte_t *pte=pgdir_walk(pgdir,va,1);
		(*pte)=page2pa(pp)|perm|PTE_P;
		return 0;
	}
	else if(pg){
		page_remove(pgdir,va);	
	}
	pte_t *pte=pgdir_walk(pgdir,va,1);
	if(pte==NULL)
		return -E_NO_MEM;
	else{
		(*pte)=page2pa(pp)|perm|PTE_P;
		pp->pp_ref++;
		}
	return 0;
}

//
// Return the page mapped at virtual address 'va'.
// If pte_store is not zero, then we store in it the address
// of the pte for this page.  This is used by page_remove and
// can be used to verify page permissions for syscall arguments,
// but should not be used by most callers.
//
// Return NULL if there is no page mapped at va.
//
// Hint: the TA solution uses pgdir_walk and pa2page.
//
struct PageInfo *
page_lookup(pde_t *pgdir, void *va, pte_t **pte_store)
{
	// Fill this function in
	struct PageInfo *result;
	pte_t *pte=pgdir_walk(pgdir,va,0);
	if(!pte_store){
		*pte_store=pte;
	}
	if(!(*pte)){
		return NULL;
	}
	else{
		result=pa2page(*pte);
	}
	return result;
}

//
// Unmaps the physical page at virtual address 'va'.
// If there is no physical page at that address, silently does nothing.
//
// Details:
//   - The ref count on the physical page should decrement.
//   - The physical page should be freed if the refcount reaches 0.
//   - The pg table entry corresponding to 'va' should be set to 0.
//     (if such a PTE exists)
//   - The TLB must be invalidated if you remove an entry from
//     the page table.
//
// Hint: The TA solution is implemented using page_lookup,
// 	tlb_invalidate, and page_decref.
//
void
page_remove(pde_t *pgdir, void *va)
{
	// Fill this function in
	pte_t* pte=NULL;
	struct PageInfo *page=page_lookup(pgdir,&va,&pte);
	if(page->pp_ref){
		page_decref(page);
	}
	(*pte)=0;
	tlb_invalidate(pgdir,va);
}

//
// Invalidate a TLB entry, but only if the page tables being
// edited are the ones currently in use by the processor.
//
void
tlb_invalidate(pde_t *pgdir, void *va)
{
	// Flush the entry only if we're modifying the current address space.
	// For now, there is only one address space, so always invalidate.
	invlpg(va);
}
