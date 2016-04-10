#include "include/string.h"
#include "include/x86.h"
#include "include/video.h"
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
	init_serial();
	init_timer();
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
	while(1){
		printg("game_main\n");
	//	printk("hello");
		init_game();
		clear_letter_pressed();
		printg("init_game");
		black_screen();
		prepare_buffer();
		draw_border();
		display_buffer();
		enable_interrupt();
		init_block();
		printg("gamestart");
		game_loop();
		sleep(2);
	}
	return 0;
}
