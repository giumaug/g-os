#ifndef SCHEDULER_H                
#define SCHEDULER_H

typedef struct s_scheduler_desc 
{
	t_llist* scheduler_queue[10];
}
t_scheduler_desc;

void schedule(struct t_processor_reg *processor_reg);
void init_scheduler();
void sched_debug();
void adjust_sched_queue(t_llist_node* node);
void _sleep(struct t_processor_reg* processor_reg);
void _awake(struct t_process_context *new_process);
_pause(struct t_processor_reg* processor_reg);
void _exit(int status,struct t_processor_reg* processor_reg);
void _exec(unsigned int start_addr,unsigned int size);
int _fork(struct t_processor_reg processor_reg);
void _sleep_time(unsigned int time,struct t_processor_reg* processor_reg);

#endif
