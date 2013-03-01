#ifndef PROCESS_H                
#define PROCESS_H

#include "console/console.h"
#include "data_types/dllist.h"

#define U_STACK_SIZE 		0x4000;
#define QUANTUM_DURATION	11931 //10 ms verificare valore piu' appropriato
#define TICK                    10;  //100ms for each process look cesati pag 260

struct t_tss {
	unsigned int* ss;
	unsigned int* esp;
};

struct t_processor_reg {
	unsigned int eax;            
	unsigned int ebx;
	unsigned int ecx;            
	unsigned int edx;                       
	unsigned int esi;            
	unsigned int edi;  
        //esp field contains kernel mode stack,user mode stack inside kernel stack
	unsigned int esp;
};

struct s_console_desc;

struct t_process_context {
	struct t_processor_reg processor_reg;
        unsigned int pid; 
	struct t_process_context* parent;
	unsigned int tick;
	struct s_console_desc *console_desc;
	unsigned int phy_add_space;
	unsigned int phy_space_size;
	char* page_dir;
	unsigned int sleep_time;
	unsigned int static_priority;
};

struct t_process_info {
	t_llist* process_context_list;
	t_llist_node* current_process;
	unsigned int next_pid;
	struct t_tss tss;	
	t_llist* pause_queue;
	struct t_processor_reg current_processor_reg;
};

#endif
