#include "include/stdio.h"
#include "include/x86.h"
/**
 * You should first implement the functions
 * printk() and vprintk()
 * in lib/printf.c
 */


/*
 * Variable panicstr contains argument to first call to panic; used as flag
 * to indicate that the kernel has already called panic.
 */
const char *panicstr;


void
_panic(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	if (panicstr)
		goto dead;
	panicstr = fmt;

	// Be extra sure that the machine is in as reasonable state
	__asm __volatile("cli; cld");

	va_start(ap, fmt);
	printk("kernel panic at %s:%d: ", file, line);
	vprintk(fmt, ap);
	printk("\n");
	va_end(ap);

dead:
	//while (1)
		//monitor(NULL);
	hlt();//dead, need to restart, you can implement the monitor and uncomment the sentence above
}

/* like panic, but don't */
void
_warn(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	va_start(ap, fmt);
	printk("kernel warning at %s:%d: ", file, line);
	vprintk(fmt, ap);
	printk("\n");
	va_end(ap);
}
