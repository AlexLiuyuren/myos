#include "include/video.h"
#include "include/mmu.h"
#include "include/system.h"
#include "include/fs.h"
#include "include/x86.h"
#include "boot/boot.h"
#include "include/string.h"
#include "include/timer.h"
#include "include/keyboard.h"
#include "include/game.h"
#include "include/semaphore.h"


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
		case initserial:
			init_serial();
			break;
		case inittimer:
			init_timer();
			break;
		case enableinterrupt:
			asm volatile("sti");
			break;
		case disenableinterrupt:
			asm volatile("cli");
			break;
		case env_fork:
			system_env_fork();
			break;
		case env_sleep:
			printk("sleep hhhhh!\n");
			system_env_sleep((uint32_t)tf->ebx);
			break;
		case env_exit:
			system_env_exit();
			break;
		//case sleep:
		case pthreadcreate:
			pthread_create((void *)tf->ebx);
			break;
		case semopen:
			semaphore_open((int)tf->ebx,(bool)tf->ecx,(int)tf->edx);
			break;
		case semclose:
			semaphore_close((int)tf->ebx);
			break;
		case semwait:
			semaphore_wait((int)tf->ebx);
			break;
 		case sempost:
			semaphore_post((int)tf->ebx);
			break;
		case semfopen:
			fs_open((const char*)tf->ebx,(int)tf->ecx);
			break;
		case semfread:
			fs_read((int)tf->ebx,(void*)tf->ecx,(int)tf->edx);
			break;
		case semfwrite:
			fs_write((int)tf->ebx,(void*)tf->ecx,(int)tf->edx);
			break;
		case semflseek:
			fs_lseek((int)tf->ebx,(int)tf->ecx,(int)tf->edx);
			break;
		case semfrewind:
			fs_rewind((int)tf->ebx);
			break;
		case semfclose:
			fs_close((int)tf->ebx);
			break;

}
}

#endif
