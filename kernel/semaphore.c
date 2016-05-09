#include "include/semaphore.h"

extern semaphore_t semaphore[1000];
void init_semaphore(){
	int i;
	for(i=0;i<1000;i++){
		semaphore[i].effective=false;
		semaphore[i].binary=false;
		semaphore[i].binary_num=0;
		semaphore[i].num=0;
		semaphore[i].wait_list=NULL;
	}
}
