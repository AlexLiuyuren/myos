#include "include/stdio.h"
#include "include/pmap.h"
#include "kernel/process/env.h"
#include "include/env.h"
#include "include/memlayout.h"
#include "include/error.h"
#include "include/string.h"

struct Env*seek_next_runnable(){
	struct Env*env;
	env=curenv;
	if(curenv->env_status==ENV_RUNNING)
		curenv->env_status=ENV_RUNNABLE;
	int i;
	for(i=0;i<=NENV+1;i++){
		if(env==&envs[NENV-1])
			env=envs;
		else
			env++;
		if(env->env_status==ENV_RUNNABLE)
			return env;

	}
	printk("no process\n");
	return NULL;	
}

void kernel_timer_event(){
	int i;
	for(i=0;i<NENV;i++){
		if(envs[i].env_status==ENV_SLEEP){
			envs[i].sleep_time-=1;
			if(envs[i].sleep_time==0)
				envs[i].env_status=ENV_RUNNABLE;
		}
	}
	struct Env* env=seek_next_runnable();
	env_run(env);
		
}

int system_env_fork(){
	int i,j;
	envid_t p_id=curenv->env_id;
	struct Env *env=NULL;
	int judge=env_alloc(&env,p_id);
	if(judge!=0){
		printk("env_alloc error!");
		return judge;
	}
	for(i=0;i<1024;i++){
		if(!(env->env_pgdir[i]&PTE_P)&&(curenv->env_pgdir[i]&PTE_P)){
		pte_t* pte=(pte_t*)(page2kva)((pa2page)(curenv->env_pgdir[i]));
		for(j=0;j<1024;j++){
			if(pte[j]){
				struct PageInfo* p=NULL;
				if(!(p=page_alloc(1)))
					return -E_NO_MEM;
				p->pp_ref++;
				page_insert(env->env_pgdir,p,(void*)(i<<22|j<<12),PTE_U|PTE_W);
				memcpy((void *)page2kva(p),(void*)page2kva(pa2page(PTE_ADDR(pte[j]))),4096);	
			}
		}
	}
	}
	env->env_tf=curenv->env_tf;
	env->env_tf.eax=0;//as the return value of process
	return env->env_id;
}

int system_pthread_create(void *p){
	int i,j;
	envid_t p_id=curenv->env_id;
	struct Env *env=NULL;
	int judge=env_alloc(&env,p_id);
	if(judge!=0){
		printk("env_alloc error!");
		return judge;
	}
	env->env_pgdir=curenv->env_pgdir;
	curenv->threadnum++;
	env->env_tf=curenv->env_tf;
	env->env_tf.esp=curenv->env_tf.esp-curenv->threadnum*PGSIZE;
	env->env_tf.eip=p;
	env->env_tf.eax=0;//as the return value of process
	return env->env_id;
}

void system_env_sleep(uint32_t time){
	curenv->env_status=ENV_SLEEP;
	curenv->sleep_time=time;
	struct Env* env=seek_next_runnable();
	env_run(env);
}

void system_env_exit(){
	curenv->env_status=ENV_DYING;
	//env_destroy(curenv);
	/*int i;
	for(i=0;i<NENV;i++){
		if(envs[i].env_status==ENV_RUNNABLE)
			printk("%d ",i);
	}
	printk("\n");
	printk("after destroy\n");*/
	struct Env* env=seek_next_runnable();
	//printk("%d\n",env-envs);
	//printk("env_id=%d,eflags=%x,eip=%x\n",env->env_id,env->env_tf.eflags,env->env_tf.eip);
	/*if(env==curenv)
		while(1);*/
	env_run(env);
}
