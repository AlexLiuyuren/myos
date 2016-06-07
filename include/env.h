#ifndef ENV_H
#define ENV_H

#include"include/types.h"
#include"include/trap.h"
#include"include/memlayout.h"
#include"include/fs.h"

typedef int32_t envid_t;
#define LOG2NENV	10
#define NENV		(1<<LOG2NENV)
#define ENVX(envid)	((envid)&(NENV))
enum{
	ENV_FREE=0,
	ENV_DYING,
	ENV_RUNNABLE,
	ENV_RUNNING,
	ENV_NOT_RUNNABLE,
	ENV_SLEEP
};

enum EnvType{
	ENV_TYPE_USER=0,
	ENV_TYPE_THREAD
};

struct Env{
	struct TrapFrame env_tf;
	struct Env*env_link;
	envid_t env_id;
	envid_t env_parent_id;
	enum EnvType env_type;
	unsigned env_status;
	uint32_t env_runs;
	uint32_t sleep_time;
	uint32_t threadnum;
	Fstate file[NR_FILES];
	//int env_cpunum;
	pde_t *env_pgdir;//kernel virtual address of page dir 
	//void *env_pgfault_upcall;

};
extern struct Env*envs;
extern struct Env*curenv;
struct Env kern_env;

#endif





