#include "include/string.h"
#include "include/x86.h"
#include "include/video.h"
#include "include/fs.h"
#include "include/game.h"
#include "include/keyboard.h"
#include "include/timer.h"
#include "include/system.h"
#include "game/include/printg.h"
//void serial_printc(int);
//void init_serial(void);

/*void draw_border(){
		
}*/
volatile int tick=0;

void timer_event(void){
	tick++;
}

void init_game(){
	//init_idt();
	//init_intr();
	//system_init_serial();
	//system_init_timer();
	printg("aaaa\n");
	printg("bbbb\n");
	syscall(0,timer_event);
	syscall(1,keyboard_event);
	//while(1);
	//printg("game start!\n");

}
void sleep(int time){
	int old_tick=tick;
	while(tick-old_tick<time*100);
}

int game_main(){
		init_game();
	while(1){
		printg("game_main\n");
	//	printk("hello");
		clear_letter_pressed();
		printg("init_game");
		black_screen();
		prepare_buffer();
		draw_border();
		display_buffer();
		//system_enable_interrupt();
		init_block();
		printg("gamestart");
		game_loop();
		sleep(2);
	}
	return 0;
}

void fork_test(){
	//if(system_env_fork()){
	//	printg("bang\n");
	//	system_env_sleep(100);
	//}
	int i;
	for(i=0;i<2;i++)
	{
	    system_env_fork();
	    printg("X\n");
	}
	while(1);
}

void sleep_test(){
	if(system_env_fork()){
		printg("aaaaa\n");
		system_env_sleep(100);
		printg("bbbbbbb\n");
	}
	while(1);
	system_env_exit();
	printg("ccccccc");
}

void exit_test(){
	//if(system_env_fork()){
	//	printg("bang\n");
	//	system_env_sleep(100);
	//}
	int i;
	for(i=0;i<2;i++)
	{
	    system_env_fork();
	    printg("X\n");
	}
	system_env_exit();
	//printg("aaa\n");
	while(1);
}


int fs_test(){
	printg("game start\n");	
	int fp1=open("poem1.txt",0);
	printg("fp1=%d\n",fp1);
	char poem[10][30];
	int i;
	for(i=0;i<4;i++){
		read(fp1,(void*)poem[i],30);
		printg("%s\n",poem[i]);
	}
	close(fp1);
	int fp2=open("poem2.txt",0);
	for(i=0;i<4;i++){
		 write(fp2,(void*)poem[i],30);
	}
	memset(poem,0,sizeof(poem));
	frewind(fp2);
	for(i=0;i<4;i++){
		read(fp2,(void*)poem[i],30);
		printg("%s\n",poem[i]);
	}
	close(fp2);
	printg("game over nice job");
	while(1);
	return 0;
 }
