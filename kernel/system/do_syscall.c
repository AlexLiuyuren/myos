#include "include/video.h"
#include "include/mmu.h"
#include "include/system.h"
#include "include/x86.h"
#include "boot/boot.h"
#include "include/string.h"
#include "include/timer.h"
#include "include/keyboard.h"


#ifndef SERIAL_PORT
#define SERIAL_PORT 0x3F8

static uint32_t *vmembase=(uint32_t*)VMEM_ADDR;
void do_syscall(TrapFrame*tf){
	switch(tf->eax){
		case drawpixel:{
			int offset=tf->ebx+tf->ecx*SCR_WIDTH;
			uint32_t *position=vmembase+offset;
			(*position)=tf->edx;
			//printk("tf->eax=%d  tf->irq=%d\n",tf->eax,tf->irq);
			}

			break;	
		case serialprint:
			while((inb(SERIAL_PORT+5)&0x20)==0);
			out_byte(SERIAL_PORT,tf->ebx);
			//printk("tf->eax=%d  tf->irq=%d\n",tf->eax,tf->irq);
			break;
		case drawpixeloff:{
			uint32_t *position=vmembase+tf->ebx;
			(*position)=tf->ecx;
			//printk("tf->eax=%d  tf->irq=%d\n",tf->eax,tf->irq);
			}
			break;
		case clearscreen:
			memset((void*)vmembase,tf->ebx,SCR_SIZE);
			//printk("tf->eax=%d  tf->irq=%d\n",tf->eax,tf->irq);
			break;
		case 0:
			set_timer_intr_handler((void*)tf->ebx);
			break;
		case 1:
			set_keyboard_intr_handler((void*)tf->ebx);
			break;
	}
}

#endif
