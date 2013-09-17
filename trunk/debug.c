#include "system.h"
#include "data_types/dllist.h"
#include "virtual_memory/vm.h"

extern t_system system;

void stop(int* stack,struct t_process_context* process_context,struct t_process_context* current_process_context)
{
	int xx;
	xx++;
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


