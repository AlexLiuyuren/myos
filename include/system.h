#define  drawpixeloff (62)
#define  drawpixel (60)
#define  serialprint (61)
#define  clearscreen (63)
int __attribute__((__noinline__)) syscall(int id,...);
void system_draw_pixel(int, int, int, int);
void system_serial_print(int, char);
void system_draw_pixel_off(int, int, int);
void system_clear_screen(int, int);
