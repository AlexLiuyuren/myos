#include"include/pmap.h"
void init_serial();
void init_idt();
void init_intr();
void init_timer();
void init_cond();
void init_mem();
void loader();
int kernel_main(){
	page_init();
	init_cond();
	return 0;
}

void init_cond(){
	printk("init");
	init_idt();
	init_intr();
	init_serial();
	init_timer();
	init_mem();
	printk("fuck");
	asm volatile("sti");
	//init_mm();
	while(1);
	//uint32_t eip=loader();
	//((void(*)(void))eip)();
	printk("shouldn't reach here");

}
