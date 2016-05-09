#include "include/system.h"
int printk(const char*fmt,...);

int __attribute__((__noinline__))
syscall(int id,...){
	int ret;
	int *args=&id;
	asm volatile("int $0x80": "=a"(ret) :"a"(args[0]),"b"(args[1]),"c"(args[2]),"d"(args[3]));
	return ret;
}


void system_draw_pixel(int x,int y,int color){
	syscall(drawpixel,x,y,color);
}

void system_serial_print(char ch){
	//printk("eax=%d ch=%d\n",eax,ch);
	//while(1);
	syscall(serialprint,ch);
}

void system_init_serial(){
	syscall(initserial);
}
void system_init_timer(){
	syscall(inittimer);
}
/*void system_sleep(int eax,int time){
	syscall(eax,time);
}*/

void system_draw_pixel_off(int off,int color){
	syscall(drawpixeloff,off,color);
}

void system_clear_screen(int color){
	syscall(clearscreen,color);
}

void system_enable_interrupt(){
	syscall(enableinterrupt);
}
void system_disenable_interrupt(){
	syscall(disenableinterrupt);
}

int system_env_fork(){
	return syscall(env_fork);
}

void system_env_sleep(uint32_t time){
	syscall(env_sleep,time);
}

void system_env_exit(){
	syscall(env_exit);
}

void system_pthread_create((void *)func){
	syscall(pthreadcreate,func);
}

void sem_open(int index,bool binary,int value){
	syscall(semopen,index,binary,value);
}

void sem_close(int index){
	syscall(semclose,index);
}
int sem_wait(int index){
	return syscall(semwait,index);
}

void sem_post(int index){
	syscall(sempost,index);
}


