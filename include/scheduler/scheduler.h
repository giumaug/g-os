#ifndef SCHEDULER_H                
#define SCHEDULER_H

#include "data_types/dllist.h"

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
void _exec(char* path);
int _fork(struct t_processor_reg processor_reg);
void _sleep_time(unsigned int time);

#endif
