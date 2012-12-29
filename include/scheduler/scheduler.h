#ifndef SCHEDULER_H                
#define SCHEDULER_H

void schedule(struct t_processor_reg *processor_reg);
void _sleep(struct t_processor_reg* processor_reg);
void _awake(struct t_process_context *new_process,struct t_processor_reg *processor_reg);
_pause(struct t_processor_reg* processor_reg);
void _exit(int status,struct t_processor_reg* processor_reg);
void _exec(unsigned int start_addr,unsigned int size);
int _fork(struct t_processor_reg processor_reg);

#endif
