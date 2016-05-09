#include "include/system.h"
#include "include/env.h"
#include "kernel/process/env.h"
void semaphore_open(int index,bool binary,int value){
	semaphore[index].effective=true;
	if(binary)
		semaphore[index].binary_num=value;
	else	
		semaphore[index].num=value;
}

void semaphore_close(int index){
	semaphore[index].effective=false;
}

int semaphore_wait(int index){
	if(semaphore[index].binary==false){
		int temp=semaphore[index].num;
		if(temp=0){
			curenv->env_status=ENV_NOT_RUNNABLE;
			curenv->env_link=semaphore[index].wait_list;
			semaphore[index].wait_list=curenv;
			struct Env* a=seek_next_runnable();
			env_run(a);
			sem_wait(index);
			return 0;
		}
		else{
			semaphore[index].num--;
			return temp;
		}
		
		}
	}
	else{
		int temp=semaphore[index].binary_num;
		if(temp==0){
			curenv->env_status=ENV_RUNNABLE;
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
	if(semaphore[index].binary==false){
		semaphore[index].num++;
		if(!semaphore[index].wait_list)
			return ;
		else{
			curenv->status=ENV_RUNNABLE;
			struct Env*a=semaphore[index].wait_list;
			wait_list=wait_list->env_link;
			a.env_status=ENV_RUNNABLE;
			env_run(a);
			return ;
		}			
	}
	else{
		semaphore[index].binary_num=1;
		if(!semaphore[index].wait_list)
			return ;
		else{
			curenv->status=ENV_RUNNABLE;
			struct Env*a=semaphore[index].wait_list;
			wait_list=wait_list->env_link;
			a.env_status=ENV_RUNNABLE;
			env_run(a);
			return ;
		}		
	}

}
