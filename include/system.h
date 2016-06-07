#include"include/types.h"
#include"include/env.h"
#define  drawpixeloff (62)
#define  drawpixel (60)
#define  serialprint (61)
#define  clearscreen (63)
#define  initserial (64)
#define  inittimer (65)
#define  enableinterrupt (66)
#define  disenableinterrupt (67)
#define  env_fork (68)
#define  env_sleep (69)
#define  env_exit (70)
#define  pthreadcreate (71)
#define  semopen (72)
#define  semclose (73)
#define  semwait (74)
#define  sempost (75)
#define  semfopen (76)
#define  semfread (77)
#define  semfwrite (78)
#define  semflseek (79)
#define  semfclose (80)
#define  semfrewind (81)
//#define  sleep (68)
int __attribute__((__noinline__)) syscall(int id,...);
void system_draw_pixel( int, int, int);
void system_serial_print(char);
void system_draw_pixel_off( int, int);
void system_clear_screen(int);
void system_init_serial();
void system_init_timer();
void system_enable_interrupt();
void system_disenable_interrupt();
int system_env_fork();
int system_pthread_create(void *);
int pthread_create(void *);
void system_env_sleep(uint32_t);
void system_env_exit();
struct Env*seek_next_runnable();
void kernel_timer_event();
void sem_open(int index,bool binary,int value);
void sem_close(int index);
int sem_wait(int index);
void sem_post(int index);
int open(const char*pathname,int flag);
int read(int fd,void *buf,int len);
int write(int fd,void *buf,int len);
int lseek(int fd,int offset,int whence);
int close(int fd);
void frewind(int fd);


//void system_sleep(int,int);
