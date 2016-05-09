#include "include/env.h"
#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#define mutex 0
#define full 1
#define empty 2
#define slotN 10

/*struct wait_list{
	struct Env* a;
	struct Env* next;
};*/

typedef struct{
	bool effective;
	bool binary;
	bool binary_num;
	int num;
	struct Env* wait_list;
}semaphore_t

void init_semaphore();
void  semaphore_open(int ,bool,int);
void semaphore_close(int );
int semaphore_wait(int);
void semaphore_post(int); 


#endif
