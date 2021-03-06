#ifndef SCHEDULER_H                
#define SCHEDULER_H

#include "system.h"

#define PID_HASH_SIZE   20
#define PGPID_HASH_SIZE 20
#define PGID_HASH_SIZE  20

//To create a new kernel thread fork with THREAD_FORK from existing one.Then from child thread use THREAD_EXEC.
//To switch kernel thread to user mode process call _exec from child thread.

//to do
//#define THREAD_EXEC(fun_pointer)	do				        \
//					{					\
//						reset stack to 1FFFFF           \
//						and call thread function        \
//	                                	from pointer 			\
//					}						

#define THREAD_FORK(pid)		do					\
					{					\
						unsigned int params[1];		\
						params[0]=0;			\
						SYSCALL(1,params);		\
						pid=params[0];			\
					}					\
					while(0)

#define	THREAD_EXIT(status)		do					\
					{					\
						int params[1];			\
						params[0]=status;		\
						SYSCALL(13,params);		\
					}					\
					while(0)

typedef struct s_scheduler_desc 
{
	t_llist* scheduler_queue[10];
}
t_scheduler_desc;

void schedule(struct t_process_context *current_process_context,struct t_processor_reg *processor_reg);
void init_scheduler();
void sched_debug();
void adjust_sched_queue(struct t_process_context *current_process_context);
void _awake();
void _pause();
void _exit(int status);
u32 _exec(char* path,char* argv[]);
int _fork(struct t_processor_reg processor_reg);
void _sleep();
void _sleep_time(unsigned int time);
void _sleep_and_unlock(t_spinlock_desc* lock);
unsigned short _getpid();
u32 _getpgid(u32 pid);
int _setpgid(u32 pid,u32 pgid);
u32 _tcgetpgrp();
u32 _tcsetpgrp(u32 fg_pgid);
void _signal();

#endif
