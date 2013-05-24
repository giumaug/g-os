#ifndef SCHEDULER_H                
#define SCHEDULER_H

typedef struct s_scheduler_desc 
{
	t_llist* scheduler_queue[10];
}
t_scheduler_desc;

void schedule();
void init_scheduler();
void sched_debug();
void adjust_sched_queue(struct t_process_context *current_process_context);
void _sleep();
void _awake();
_pause();
void _exit(int status);
void _exec(unsigned int start_addr,unsigned int size);
int _fork();
void _sleep_time(unsigned int time);

#endif
