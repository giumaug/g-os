#ifndef PROCESS_H                
#define PROCESS_H

#include "system.h"
#include "console/console.h"
#include "data_types/dllist.h"
#include "data_types/hashtable.h"
#include "data_types/primitive_types.h"
#include "ext2/ext2.h"

#define U_STACK_SIZE 		0x4000;
//#define QUANTUM_DURATION	11931 //10 ms verificare valore piu' appropriato
#define TICK                    10;  //100ms for each process look cesati pag 260
#define CURRENT_PROCESS_CONTEXT(current_process_context) do {t_llist_node* node;node=system.process_info->current_process;current_process_context=node->val; } while (0);
#define KERNEL_THREAD 0
#define USERSPACE_PROCESS 1

typedef enum s_proc_status
{ 
	RUNNING,
	SLEEPING,
	EXITING
} 
t_proc_status;

struct t_tss 
{
	unsigned int* ss;
	unsigned int* esp;
};

struct t_processor_reg 
{
	unsigned int eax;            
	unsigned int ebx;
	unsigned int ecx;            
	unsigned int edx;                       
	unsigned int esi;            
	unsigned int edi;  
        //esp field contains kernel mode stack,user mode stack inside kernel stack
	unsigned int esp;
	unsigned int ds;
	unsigned int es;
};

struct t_process_context 
{
	struct t_processor_reg processor_reg;
        unsigned int pid; 
	struct t_process_context* parent;
	unsigned int tick;
	struct s_console_desc *console_desc;
//	unsigned int phy_add_space;
	//0 thread 1 process
	u32 process_type;
	unsigned int phy_space_size;
	char* page_dir;
	int sleep_time;
	//when io queue manager implemented create a sleep manager too and move
	//this filed inside it
	int assigned_sleep_time;
	unsigned int static_priority;
	unsigned int curr_sched_queue_index;
	t_proc_status proc_status;
	u32 root_dir_inode_number;
	u32 current_dir_inode_number;
	u32 next_fd;
	t_hashtable* file_desc;
	unsigned int phy_kernel_stack;
	unsigned int phy_user_stack;
	t_mem_reg* process_mem_reg;
	t_mem_reg* heap_mem_reg;
	t_mem_reg* ustack_mem_reg;

};

struct t_process_info 
{
	t_llist* process_context_list;
	t_llist_node* current_process;
	unsigned int next_pid;
	struct t_tss tss;	
	t_llist* pause_queue;
	t_llist* sleep_wait_queue;
	struct t_processor_reg current_processor_reg;
	u32 heap_start_addr;
	u32 heap_size;
	t_llist* mem_regs;
};

#endif
