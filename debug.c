#include "general.h"
#include "data_types/dllist.h"
#include "virtual_memory/vm.h"
#include "memory_manager/general.h"
#include "debug.h"

extern unsigned int free_mem_list[POOL_NUM];

unsigned int proc[100];
int t_sched_debug[10][10];
int index_2=0;

void check_free_mem()
{
	int i=0;
	unsigned int buddy_mem;
	unsigned int pool_mem;
	buddy_mem=buddy_free_mem(system.buddy_desc);
	pool_mem=kfree_mem();
	//buddy_check_mem_status(system.buddy_desc);
	//a_fixed_size_check_mem_status();
	check_active_process();
	if (buddy_mem!=160194560)
	{
		panic();
	}
	//279852
	if (pool_mem!=279850)
	{
		panic();
	}
}

check_process_context()
{
	t_llist_node* next;
	t_llist_node* sentinel_node;
	struct t_process_context* process_context;
	int index=0;

	while(index<10)
	{
		sentinel_node=ll_sentinel(system.scheduler_desc.scheduler_queue[index]);
		next=ll_first(system.scheduler_desc.scheduler_queue[index]);
		while(next!=sentinel_node)
		{
			process_context=next->val;
				if (process_context->curr_sched_queue_index>9 || process_context->curr_sched_queue_index<0)
				{
					panic();
					return;
				}
				next=next=ll_next(next);
		}
		index++;
	}
}

check_active_process()
{
	t_llist_node* next;
	t_llist_node* sentinel_node;
	struct t_process_context* process_context;
	int index=0;
	
	index_2=0;


	while(index<10)
	{
		sentinel_node=ll_sentinel(system.scheduler_desc.scheduler_queue[index]);
		next=ll_first(system.scheduler_desc.scheduler_queue[index]);
		while(next!=sentinel_node)
		{
			process_context=next->val;
			proc[index_2]=process_context->pid;
			index_2++;
			next=next=ll_next(next);
		}
		index++;
	}
}
