#include "include/env.h"
#include "include/mmu.h"
#include "include/memlayout.h"
#include "include/error.h"
#include "include/pmap.h"
#include "include/types.h"
#include "include/trap.h"
#include "include/elf.h"
#include "include/x86.h"
#include "include/mmu2.h"
#include "include/env.h"
#include "include/string.h"
#include "kernel/process/env.h"
struct Env* envs=NULL;
struct Env* curenv=NULL;
static struct Env* env_free_list;
extern pde_t entry_pgdir[];  
pde_t *kern_pgdir=entry_pgdir;
//extern SegDesc gdt[];
#define ENVGENSHIFT 12

/*struct Segdesc gdt[]={
	SEG_NULL,
	[GD_KT>>3]=SEG(STA_X|STA_R,0x0,0xffffffff,0),
	[GD_KD>>3]=SEG(STA_W,0x0,0xffffffff,0),
	[GD_UT>>3]=SEG(STA_X|STA_R,0x0,0xffffffff,3),
	[GD_UD>>3]=SEG(STA_W,0x0,0xffffffff,3),
	[GD_TSS0>>3]=SEG_NULL
};*/

/*struct Pseudodesc gdt_pd={
	sizeof(gdt)-1,(unsigned long)gdt
};*/


int envid2env(envid_t envid,struct Env **env_store, bool checkperm){
	struct Env*e;
	if(envid==0){
		*env_store=curenv;
		return 0;
	}
	e=&envs[ENVX(envid)];
	if(e->env_status==ENV_FREE||e->env_id!=envid){
		*env_store=0;
		return -E_BAD_ENV;
	}
	if(checkperm&& e!=curenv && e->env_parent_id !=curenv->env_id){
		*env_store=0;
		return -E_BAD_ENV;
	}
	*env_store=e;
	return 0;
}

void env_init(void){
	int i;
	for(i=NENV-1;i>=0;i--){
		envs[i].env_id=0;
		envs[i].env_link=env_free_list;
		env_free_list=envs+i; 
	}
	//env_init_percpu();
	return ;
}
//load GDT and segment descriptor
/*void env_init_percpu(void){
	lgdt(&gdt_pd);
	asm volatile("movw %%ax,%%gs"::"a"(GD_UD|3));
	asm volatile("movw %%ax,%%fs"::"a"(GD_UD|3));
	
	asm volatile("movw %%ax,%%es"::"a"(GD_KD));
	asm volatile("movw %%ax,%%ds"::"a"(GD_KD));
	asm volatile("movw %%ax,%%ss"::"a"(GD_KD));
	
	asm volatile("ljmp %0, $1f\n 1:\n"::"i"(GD_KT));
	lldt(0);
}*/


static int env_setup_vm(struct Env*e){
	//int i;
	struct PageInfo *p=NULL;
	if(!(p=page_alloc(ALLOC_ZERO)))
		return -E_NO_MEM;
	p->pp_ref++;
	e->env_pgdir=(pde_t *)page2kva(p);
	memcpy(e->env_pgdir,kern_pgdir,PGSIZE);
	e->env_pgdir[PDX(UVPT)]=PADDR(e->env_pgdir)|PTE_P|PTE_U;
	return 0;
}

int env_alloc(struct Env**newenv_store,envid_t parent_id){
	int32_t generation;
	int r;
	struct Env *e;
	if(!(e=env_free_list))
		return -E_NO_FREE_ENV;
	if((r=env_setup_vm(e))<0)
		return r;
	generation=(e->env_id+(1<<ENVGENSHIFT))& ~(NENV-1);
	if(generation<=0)
		generation=1<<ENVGENSHIFT;
	e->env_id=generation|(e-envs);
	printk("envs:%x,e:%x,e->env_id:%x\n",envs,e,e->env_id);
	e->env_parent_id=parent_id;
	e->env_type=ENV_TYPE_USER;
	e->env_status=ENV_RUNNABLE;
	e->env_runs=0;
	
	memset(&e->env_tf,0,sizeof(e->env_tf));
	e->env_tf.ds=GD_UD|3;
	e->env_tf.es=GD_UD|3;
	e->env_tf.ss=GD_UD|3;
	e->env_tf.esp=USTACKTOP;
	e->env_tf.cs=GD_UT|3;

	env_free_list=e->env_link;
	*newenv_store=e;

	return 0;
}

static void region_alloc(struct Env*e, void *va, size_t len){
	void *begin=ROUNDDOWN(va,PGSIZE);
	void *end=ROUNDUP(va+len,PGSIZE);
	for(;begin<end;begin+=PGSIZE){
		struct PageInfo *pg=page_alloc(0);
		if(!pg){ 
			printk("region alloc failed!");
			//panic("region alloc failed!");
			return ;
		}
		page_insert(e->env_pgdir,pg,begin,PTE_W|PTE_U);
	}
}

static void load_icode(struct Env*e,uint8_t *binary,size_t size){
	struct Elf* ELFHDR=(struct Elf*)binary;
	struct Proghdr *ph,*eph;
	if(ELFHDR->e_magic!=ELF_MAGIC){
		printk("magic error\n");
		return ;
	}
		//panic("magic error");

	ph=(struct Proghdr*)((uint8_t *)ELFHDR+ELFHDR->e_phoff);
	eph=ph+ELFHDR->e_phnum;
	lcr3(PADDR(e->env_pgdir));
	for(;ph<eph;ph++){
		if(ph->p_type==ELF_PROG_LOAD){
			region_alloc(e,(void*)ph->p_va,ph->p_memsz);
			memset((void*)ph->p_va,0,ph->p_memsz);
			memcpy((void*)ph->p_va,binary+ph->p_offset,ph->p_filesz);
		}
	}
	lcr3(PADDR(kern_pgdir));
	e->env_tf.eip=ELFHDR->e_entry;
	region_alloc(e,(void*)(USTACKTOP-PGSIZE),PGSIZE);
}

void env_create(uint8_t *binary, size_t size, enum EnvType type){
	struct Env *penv;
	env_alloc(&penv,0);
	load_icode(penv,binary,size);
}

void env_free(struct Env* e){
	pte_t *pt;
	uint32_t pdeno,pteno;
	physaddr_t pa;
	if(e==curenv)
		lcr3(PADDR(kern_pgdir));
	//static_assert(UTOP%PTSIZE==0);
	for(pdeno=0;pdeno<PDX(UTOP);pdeno++){
		if(!(e->env_pgdir[pdeno])&PTE_P)
			continue;
		pa=PTE_ADDR(e->env_pgdir[pdeno]);
		pt=(pte_t*)KADDR(pa);
		for(pteno=0;pteno<=PTX(~0);pteno++){
			if(pt[pteno]&PTE_P)
				page_remove(e->env_pgdir,PGADDR(pdeno,pteno,0));
				
		}
		e->env_pgdir[pdeno]=0;
		page_decref(pa2page(pa));
	}
	pa=PADDR(e->env_pgdir);
	e->env_pgdir=0;
	page_decref(pa2page(pa));

	e->env_status=ENV_FREE;
	e->env_link=env_free_list;
	env_free_list=e;
}

void env_destroy(struct Env *e){
	env_free(e);
	while(1);
}


void env_pop_tf(struct TrapFrame* tf){
	 asm volatile(
		"movl %0,%%esp\n"
		"\tpopal\n"
		"\tpopl %%es\n"
		"\tpopl %%ds\n"
		"\taddl $0x8,%%esp\n"
		"\tiret"
		::"g"(tf):"memory");
	//panic("iret failed");
	printk("iret failed\n"); 
}

void env_run(struct Env* e){
	if(curenv!=e){
		curenv=e;
		e->env_status=ENV_RUNNING;
		e->env_runs++;
		lcr3(PADDR(e->env_pgdir));
	}
	env_pop_tf(&e->env_tf);
}





