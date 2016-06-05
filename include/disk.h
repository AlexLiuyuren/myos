#ifndef DISK_H
#define DISK_H

#include "include/env.h"
#include "include/x86.h"
void waitdisk();
void readsect(void *dst, int offset);
void readseg(unsigned char *pa, int count, int offset);
void writesect(void*src, int offset);


#endif

