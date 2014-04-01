#ifndef DEBUG_H                
#define DEBUG_H

//#define PROC_PID ((struct t_process_context*) system.process_info.current_process->val)->pid
//#define TRACE(POINT,PID) system.tracepoint_index++;                    \
//		if (system.tracepoint_index>200000)                    \
//		{                                                      \
//			panic();                                       \
//		}		                                       \
//		system.tracepoint[system.tracepoint_index].pid=PID;    \
//	       system.tracepoint[system.tracepoint_index].point=POINT;
//
//typedef struct s_tracepoint 
//{
//	unsigned int pid;
//	unsigned int point;
//}
//t_tracepoint;

void check_race(int i);
void panic();
void stop(int* stack,struct t_process_context* fault_process_context,struct t_process_context* current_process_context);
void check_free_mem();
void check_stack_change();

#endif                                
