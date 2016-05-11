#include "include/system.h"
#include "include/stdio.h"
#include "include/semaphore.h"
#include "include/env.h"
#include "kernel/process/env.h"

void semaphore_open(int index,bool binary,int value){
	semaphore[index].effective=true;
	semaphore[index].binary=binary;
	if(binary)
		semaphore[index].binary_num=(bool)value;
	else	
		semaphore[index].num=value;
}

void semaphore_close(int index){
	semaphore[index].effective=false;
}

int semaphore_wait(int index){
//	int i;
	if(semaphore[index].effective==false){
		printk("semaphore invalued\n");
		return -1;
	}
/*	for(i=0;i<3;i++){
		printk("semaphore:%d %d %d\n",semaphore[0].binary_num,semaphore[1].num,semaphore[2].num);
	}*/
	if(semaphore[index].binary==false){
		int temp=semaphore[index].num;
		//printk("semaphore num=%d\n",temp);
		if(temp==0){
			curenv->env_status=ENV_NOT_RUNNABLE;
			curenv->env_link=semaphore[index].wait_list;
			semaphore[index].wait_list=curenv;
			struct Env* a=seek_next_runnable();
			env_run(a);
			return 0;
		}
		else{
			semaphore[index].num--;
			return temp;
		}	
	}
	else{
		int temp=semaphore[index].binary_num;
		//printk("binarynum=%d\n",temp);
		if(temp==0){
			curenv->env_status=ENV_NOT_RUNNABLE;
			curenv->env_link=semaphore[index].wait_list;
			semaphore[index].wait_list=curenv;
			struct Env* a=seek_next_runnable();
			env_run(a);
			return 0;
		}
		else{
			semaphore[index].binary_num=0;
			return temp;
		}
	}
		
}

void semaphore_post(int index){
	if(semaphore[index].effective==false){
		printk("semaphore invalued\n");
		return ;
	}
	if(semaphore[index].binary==false){
		if(!semaphore[index].wait_list){
			semaphore[index].num++;
			return ;
		}
		else{
			curenv->env_status=ENV_RUNNABLE;
			struct Env*a=semaphore[index].wait_list;
			semaphore[index].wait_list=semaphore[index].wait_list->env_link;
			a->env_status=ENV_RUNNABLE;
			env_run(a);
			return ;
		}			
	}
	else{
		if(!semaphore[index].wait_list){
			semaphore[index].binary_num=1;
			return ;
		}
		else{
			curenv->env_status=ENV_RUNNABLE;
			struct Env*a=semaphore[index].wait_list;
			semaphore[index].wait_list=semaphore[index].wait_list->env_link;
			a->env_status=ENV_RUNNABLE;
			env_run(a);
			return ;
		}		
	}

}
