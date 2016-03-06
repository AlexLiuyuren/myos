#include "./include/string.h"
#include "./include/x86.h"
#include "./include/video.h"

void serial_printc(int);
void init_serial(void);
int main(){
	blue_screen();
	//init_serial();
	//serial_printc('a');
	hlt();
	return 0;
}
