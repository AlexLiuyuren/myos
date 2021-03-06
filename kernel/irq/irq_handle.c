#include "include/x86.h"
#include "include/stdio.h"
#include "include/assert.h"
#include "include/mmu.h"
#include "include/env.h"
//#include "game.h"

static void (*do_timer)(void);
static void (*do_keyboard)(int);
void do_syscall(TrapFrame*tf);

void
set_timer_intr_handler( void (*ptr)(void) ) {
	do_timer = ptr;
}
void
set_keyboard_intr_handler( void (*ptr)(int) ) {
	do_keyboard = ptr;
}

/* TrapFrame的定义在include/memory.h
 * 请仔细理解这段程序的含义，这些内容将在后续的实验中被反复使用。 */
void
irq_handle(struct TrapFrame *tf) {
	curenv->env_tf=*tf;
	if(tf->irq==0x80){
		do_syscall(tf);
	}
	else if(tf->irq < 1000) {
		if(tf->irq == -1) {
			printk("%s, %d: Unhandled exception!\n", __FUNCTION__, __LINE__);
		}
		else {
			printk("%s, %d: Unexpected exception #%d!\n", __FUNCTION__, __LINE__, tf->irq);
		}
		assert(0);
	}

	else if (tf->irq == 1000) {
		//printk("time\n");
		if(do_timer)
			do_timer();
	} 
	else if (tf->irq == 1001) {
		uint32_t code = inb(0x60);
		uint32_t val = inb(0x61);
		outb(0x61, val | 0x80);
		outb(0x61, val);
	       //printk("hello");
		printk("%s, %d: key code = %x\n", __FUNCTION__, __LINE__, code);
		if(do_keyboard)
			do_keyboard(code);
	}
	else if(tf->irq== 1014){
		
	}
	else {
		assert(0);
	}
}

