#include "include/system.h"
#include "include/semaphore.h"
#include "game/include/printg.h"
int items[10];
int itemid;
void producer(void){
	int i;
	while(1){
		printg("producer: \n");
		sem_wait(empty);
		sem_wait(mutex);
		items[itemid++]=1;
		for(i=0;i<10;i++){
			printg("%d ",items[i]);
		}
		printg("\n");
		sem_post(mutex);
		sem_post(full);
	}
}

void consumer(void){
	int i;
	while(1){
		printg("consumer: \n");
		sem_wait(full);	
		sem_wait(mutex);
		items[--itemid]=0;
		for(i=0;i<10;i++){
			printg("%d ",items[i]);
		}
		printg("\n");
		sem_post(mutex);
		sem_post(empty);
	} 
}
int pc_main(){
	int i;
	itemid=0;
	printg("pc_main:\n");
	sem_open(mutex,true,1);
	sem_open(empty,false,slotN);
	sem_open(full,false,0);
	for(i=0;i<10;i++){
		items[i]=0;
	}
	for(i=0;i<4;i++){
		system_pthread_create(producer);
		system_pthread_create(consumer);
	}
	while(1);
}
