#include"include/pmap.h"
#include"include/game.h"
#include"include/string.h"
#include"include/x86.h"
#include"include/system.h"
#include"include/semaphore.h"
#include"include/memlayout.h"
#include"kernel/process/env.h"

#include "include/mmu.h"
extern pde_t entry_pgdir[];
void init_cond();
void init_segment();
void init_mem();
void* loader();
int kernel_main(){
	page_init();
	init_cond();
	return 0;
}
extern struct PageInfo pages[];
//extern char end[];
void init_cond(){
	init_segment();
	init_idt();
	init_intr();
	init_serial();
	init_timer();
	init_mem();
	init_semaphore();
	set_timer_intr_handler(kernel_timer_event);
	asm volatile("cli");
	//asm volatile("int $14");
	env_init();
	env_create();
	env_run(&envs[0]);
	/*printk("snvs0.ts=%x\n",&envs[0]);
	struct TrapFrame tf;
	tf.es = 0x23;
	env_pop_tf(&tf);
	envs[0].env_tf.edi = 1;
	envs[0].env_tf.esi = 2;
	envs[0].env_tf.ebp = 3;
	envs[0].env_tf.esp = 4;
	envs[0].env_tf.es= 4;*/
	/*struct PageInfo *page=page_alloc(1);
	uint32_t cr3_game=page2pa(page);
	pde_t *pgdir_game=page2kva(page);
	memcpy(pgdir_game,entry_pgdir,4096);
	printk("pages=%x",pages);
	void* eip=loader(pgdir_game);
	lcr3(cr3_game);
	printk("eip=%x\n",eip);
	((void(*)(void))eip)();
	printk("shouldn't reach here");*/
	
}
