#include "include/video.h"
#include "include/mmu.h"
#include "include/system.h"
#include "include/x86.h"
#include "boot/boot.h"
#include "include/string.h"

#ifndef SERIAL_PORT
#define SERIAL_PORT 0x3F8

static uint8_t *vmembase=VMEM_ADDR;
void do_syscall(TrapFrame*tf){
	switch(tf->eax){
		case drawpixel:{
			int offset=tf->ebx+tf->ecx*SCR_WIDTH;
			uint8_t *position=vmembase+offset;
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
			uint8_t *position=vmembase+tf->ebx;
			(*position)=tf->ecx;
			//printk("tf->eax=%d  tf->irq=%d\n",tf->eax,tf->irq);
			}
			break;
		case clearscreen:
			memset((void*)vmembase,tf->ebx,SCR_SIZE);
			//printk("tf->eax=%d  tf->irq=%d\n",tf->eax,tf->irq);
			break;
	}
}

#endif
