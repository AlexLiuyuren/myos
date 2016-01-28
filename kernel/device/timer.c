#include "include/x86.h"
#include "include/assert.h"
#include "include/timer.h"

/* 8253输入频率为1.193182MHz */
#define TIMER_PORT 0x40
#define FREQ_8253 1193182
#define HZ 100

void
init_timer(void) {
	;
}
