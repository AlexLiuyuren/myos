#include"include/pmap.h"
#include"include/game.h"
#include"include/string.h"
#include"include/x86.h"
#include"include/memlayout.h"
extern pde_t entry_pgdir[];
void init_cond();
void init_segment();
void init_mem();
void* loader();
int kernel_main(){
	printk("before page init\n");
	page_init();
	printk("after page init\n");
	init_cond();
	printk("after condition init\n");
	return 0;
}
extern struct PageInfo pages[];
//extern char end[];
void init_cond(){
//	init_segment();
	init_idt();
	init_intr();
	init_serial();
	init_timer();
	init_mem();
	//printk("end=%x\n",end);
	//asm volatile("sti");
	//to store kernel pgd
	struct PageInfo *page=page_alloc(1);
	uint32_t cr3_game=page2pa(page);
	pde_t *pgdir_game=page2kva(page);
	memcpy(pgdir_game,entry_pgdir,4096);
	//int *p = (int *)0xa0000;
	//*p = 0;
	//while(1);
	printk("pages=%x",pages);
	void* eip=loader(pgdir_game);
	lcr3(cr3_game);
	printk("eip=%x\n",eip);
	((void(*)(void))eip)();
	printk("shouldn't reach here");

}
