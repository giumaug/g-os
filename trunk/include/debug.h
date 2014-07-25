#ifndef DEBUG_H                
#define DEBUG_H

void check_race(int i);
void panic();
void stop(int* stack,struct t_process_context* fault_process_context,struct t_process_context* current_process_context);
void check_free_mem();
void check_stack_change();

#endif                                
