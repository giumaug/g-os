#ifndef SCHEDULER_H                
#define SCHEDULER_H

#include "data_types/dllist.h"

#define THREAD_EXEC(path,args) 	do					\
			 	{					\
					unsigned int params[2];		\
					params[0]=path;			\
					params[1]=args                  \
					SYSCALL(14,params);		\
				}					\
				while(0)

#define THREAD_FORK(pid)	do					\
				{					\
					unsigned int params[2];		\
					params[0]=NO_INIT_VM_USERSPACE;	\
					params[1]=0;			\
					SYSCALL(1,params);		\
					pid=params[1];			\
				}					\
				while(0)

#define	THREAD_EXIT(status)	do					\
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
void _sleep();
void _awake();
void _pause();
void _exit(int status);
void _exec(char* path,char* argv[]);
int _fork(struct t_processor_reg processor_reg,unsigned int flags);
void _sleep_time(unsigned int time);

#endif
