#include "./include/string.h"
#include "./include/x86.h"
#include "./include/video.h"
#include "./include/game.h"
#include "kernel/game/include/printg.h"
//void serial_printc(int);
//void init_serial(void);

/*void draw_border(){
		
}*/
volatile int tick=0;

void timer_event(void){
	tick++;
}

void init_game(){
	init_idt();
	init_intr();
	init_serial();
	init_timer();
	set_timer_intr_handler(timer_event);
	set_keyboard_intr_handler(keyboard_event);
	//while(1);
	//printg("game start!\n");

}
void sleep(int time){
	int old_tick=tick;
	while(tick-old_tick<time*100);
}

int main(){
	while(1){
		init_game();
		black_screen();
		prepare_buffer();
		draw_border();
		display_buffer();
		enable_interrupt();
		init_block();
		game_loop();
		sleep(2);
	}
	return 0;
}
