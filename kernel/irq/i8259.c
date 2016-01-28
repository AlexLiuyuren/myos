#include "include/x86.h"

#define PORT_PIC_MASTER 0x20
#define PORT_PIC_SLAVE  0xA0
#define IRQ_OFFSET 0x20
#define IRQ_SLAVE       2

/* 初始化8259中断控制器：
 * 硬件中断IRQ从32号开始，自动发送EOI */
void
init_intr(void) {
	hlt();
	/*
	 * uncomment hlt() after implement
	 *
	 */
}
