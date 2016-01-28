#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "include/types.h"
#include "include/assert.h"

#define SCR_WIDTH  320
#define SCR_HEIGHT 200
#define SCR_SIZE ((SCR_WIDTH) * (SCR_HEIGHT))
#define VMEM_ADDR  ((uint8_t*)0xA0000)

extern uint8_t *vmem;

static inline void
draw_pixel(int x, int y, int color) {
	assert(x >= 0 && y >= 0 && x < SCR_HEIGHT && y < SCR_WIDTH);
	vmem[(x << 8) + (x << 6) + y] = color;
}

void prepare_buffer();
void display_buffer();
void blue_screen();

void draw_string(const char*, int, int, int);
#endif
