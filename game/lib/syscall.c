#include "include/system.h"
int printk(const char*fmt,...);

int __attribute__((__noinline__))
syscall(int id,...){
	int ret;
	int *args=&id;
	asm volatile("int $0x80": "=a"(ret) :"a"(args[0]),"b"(args[1]),"c"(args[2]),"d"(args[3]));
	return ret;
}


void system_draw_pixel(int eax,int x,int y,int color){
	syscall(eax,x,y,color);
}

void system_serial_print(int eax,char ch){
	//printk("eax=%d ch=%d\n",eax,ch);
	//while(1);
	syscall(eax,ch);
}

void system_draw_pixel_off(int eax,int off,int color){
	syscall(eax,off,color);
}

void system_clear_screen(int eax,int color){
	syscall(eax,color);
}
