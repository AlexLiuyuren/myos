#include "include/x86.h"
#include "include/stdarg.h"
#include "include/stdio.h"

/*
 * You may refer to lib/printfmt.c
 * to implement the printk() and vprintk().
 * You can also implement a simplier version
 * by yourself.
 */

static void
/*putch(int ch, int *cnt){
	cputchar(ch);
	*cnt++;
}*/


int	vprintk(const char *fmt,va_list ap){ 
	//hlt();
	/*
	 * uncomment the hlt()
	 * after your completement.
	 * refer to manual.
	 */
	int cnt=0;
	vprintfmt((void*)putch,&cnt,fmt,ap);
	return cnt;
}

int vcprintf(const char *fmt, va_list ap){
	int cnt=0;
	vprintfmt((void*)putch,&cnt,fmt,ap);
	return cnt;
}

int	printk(const char *fmt, ...){
	//hlt();
	/*
	 * uncomment the hlt()
	 * after your completement.
	 * refer to manual.
	 *
	 * Hint:Use va_list to get the parameters and call vprintk().
	 *		You may also build the actual string here and call a
	 *		function which would print a string.
	 */
	va_list ap;
	int cnt;
	va_start(ap,fmt);
	cnt=vcprintf(fmt,ap);
	va_end(ap);
	return cnt;
}
