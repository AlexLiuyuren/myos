#include "include/x86.h"
#include "include/stdarg.h"
#include "include/stdio.h"
#include "include/system.h"

static void 
putch_g(int ch,int *cnt){
	system_serial_print(ch);
	(*cnt)++;
}

int vprintg(const char *fmt,va_list ap){
	int cnt=0;
	vprintfmt((void*)putch_g,&cnt,fmt,ap);
	return cnt;
}

int printg(const char *fmt,...){
	va_list ap;
	int cnt;
	va_start(ap,fmt);
	cnt=vprintg(fmt,ap);
	va_end(ap);
	return cnt;
}

