#include "system.h"
#include "data_types/dllist.h"
#include "virtual_memory/vm.h"
#include "memory_manager/general.h"
#include "debug.h"

extern t_system system;
extern unsigned int free_mem_list[POOL_NUM];
unsigned int exit_count_1=0;
unsigned int exit_count_2=0;
unsigned int exit_count_3=0;
unsigned int exit_count_4=0;
unsigned int exit_count_5=0;
unsigned int exit_count_6=0;
unsigned int exit_count_7=0;
unsigned int exit_count_8=0;
unsigned int exit_count_9=0;
unsigned int exit_count_10=0;

unsigned int proc[100];
int t_sched_debug[10][10];
int index_2=0;

void panic()
{
	return;
}

void stop(int* stack,struct t_process_context* fault_process_context,struct t_process_context* current_process_context)
{
	int xx;
	xx++;
}

void check_free_mem()
{
	int i=0;
	unsigned int buddy_mem;
	unsigned int pool_mem;
	buddy_mem=buddy_free_mem(&system.buddy_desc);
	pool_mem=kfree_mem();
	//buddy_check_mem_status(&system.buddy_desc);
	//a_fixed_size_check_mem_status();
	check_active_process();
	if (buddy_mem!=160194560)
	//if (buddy_mem!=21934080)
	{
		panic();
	}
	if (pool_mem!=279852)
	{
		panic();
	}
	if (exit_count_2<(exit_count_1-1))
	{
		panic();
	}
}

void check_stack_change()
{
	struct t_process_context* current_process_context;
	struct t_process_context* next_process_context;
	t_llist_node* node;
	t_llist_node* next;
	t_llist_node* sentinel_node;
	int index=0;
	node=system.process_info.current_process;	
	current_process_context=node->val;
	int* stack;
	unsigned int tmp;
		
	while(index<10)
	{
		sentinel_node=ll_sentinel(system.scheduler_desc.scheduler_queue[index]);
		next=ll_first(system.scheduler_desc.scheduler_queue[index]);
		while(next!=sentinel_node)
		{
			next_process_context=next->val;
			tmp=FROM_PHY_TO_VIRT(next_process_context->phy_add_space);//BUDDY_START_ADDR + VIRT_MEM_START_ADDR;
			stack=FROM_PHY_TO_VIRT(next_process_context->phy_add_space)+0xffffb;
			if ((*stack)!=0x23)
			{
				stop(stack,next_process_context,current_process_context);
			}
			next=ll_next(next);	
		}
		index++; 
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

void sched_debug()
{
	struct t_process_context* next_process_context;
	t_llist_node* next;
	t_llist_node* sentinel_node;
	unsigned int i,j;

	for (i=0;i<10;i++)
	{
		for (j=0;j<10;j++)
		{
			t_sched_debug[i][j]=-1;
		}
	}
	for (i=0;i<10;i++)
	{
		sentinel_node=ll_sentinel(system.scheduler_desc.scheduler_queue[i]);
		next=ll_first(system.scheduler_desc.scheduler_queue[i]);
		j=0;
		while(next!=sentinel_node)
		{
			next_process_context=next->val;
			t_sched_debug[i][j++]=next_process_context->pid;
			next=ll_next(next);	
		} 
	}
	return;
}
