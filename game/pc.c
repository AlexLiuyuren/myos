#include "include/system.h"
#include "include/semaphore.h"
#include "game/include/printg.h"
int items[10];

void producer(void){
	int temp,i;
	while(1){
		temp=sem_wait(empty);
		sem_wait(mutex);
		items[temp]=1;
		printg("producer: ");
		for(i=0;i<10;i++){
			printg("%d ",items[i]);
		}
		printg("\n");
		sem_post(mutex);
		sem_post(full);
	}
}

void consumer(void){
	int temp;
	while(1){
		temp=sem_wait(full);	
		sem_wait(mutex);
		items[temp]=0;
		printg("consumer: ");
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
