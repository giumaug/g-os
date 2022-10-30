#include "drivers/pit/8253.h"
#include "drivers/lapic/lapic.h"
#include "asm.h"  
#include "idt.h" 
#include "timer.h" 
//#include "virtual_memory/vm.h"
//#include "drivers/pic/8259A.h" 

//#define K_STACK 0x1FFFFB

int static status = 0;

int counter = 0;

void init_pit()
{	
	static struct t_i_desc i_desc;
	out(BINARIY_COUNT_MODE,CONTROL_WORD);
	out(LOW_QUANTUM,COUNTER_0);
	out(HI_QUANTUM,COUNTER_0);
	i_desc.baseLow=((int)&int_handler_pit) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00; //0x0EF00;
	i_desc.baseHi=((int)&int_handler_pit)>>0x10;
	status = 1;
	set_idt_entry(0x22,&i_desc);
}

void free_pit()
{
	out(LOW_QUANTUM,COUNTER_0);
	out(HI_QUANTUM,COUNTER_0);
	status = 0;	
}

void int_handler_pit()
{
	t_llist_node* sentinel_node = NULL;
	t_llist_node* node = NULL;
	t_timer* timer = NULL;
	struct t_processor_reg processor_reg;

	SAVE_PROCESSOR_REG
	//EOI_TO_LAPIC
	SWITCH_DS_TO_KERNEL_MODE
	counter++;
	if (status == 1)
	{
		sentinel_node = ll_sentinel(system.timer_list);
		node = ll_first(system.timer_list);
		while(node != sentinel_node)
		{
			timer = node->val;
			timer->val --;
			if (timer->val <= 0 )
			{
				//(*timer->handler)(timer->handler_arg);
				break;
			}
			node = ll_next(node);
		}
	}

	EOI_TO_LAPIC
	// tmp put for test fake console. Put EXIT_INT_HANDLER
	//_processor_reg = processor_reg;
	//RESTORE_PROCESSOR_REG                                                                                
	//RET_FROM_INT_HANDLER
	EXIT_INT_HANDLER(0, processor_reg)
}

/*
void __int_handler_pit()
{
	int is_schedule=0;
	struct t_process_context* process_context;
	struct t_process_context* sleeping_process;
	struct t_processor_reg processor_reg;
	t_llist_node* next;
	t_llist_node* sentinel;
	t_llist_node* old_node;
	struct t_process_context* next_process;
	unsigned int queue_index;
	unsigned int priority;
	t_llist_node* sentinel_node;
	t_llist_node* node;
	t_llist_node* first_node;
	t_timer* timer;
	
	SAVE_PROCESSOR_REG
	EOI_TO_MASTER_PIC
	SWITCH_DS_TO_KERNEL_MODE
	process_context = system.process_info->current_process->val;

	system.time+=QUANTUM_DURATION;
	if (system.int_path_count>0)
	{
		goto exit_handler;
	}
	sleeping_process=system.active_console_desc->sleeping_process;
	sentinel=ll_sentinel(system.process_info->sleep_wait_queue);
	next=ll_first(system.process_info->sleep_wait_queue);
	next_process=next->val;
	//THIS STUFF MUST BE MOVED INSIDE ASSIGNED SLEEP MANAGER LIKE IO
	while(next!=sentinel)
	{
		next_process->assigned_sleep_time-=QUANTUM_DURATION;
		next_process->sleep_time+=QUANTUM_DURATION;
		
		if (next_process->sleep_time>1000) 	
		{
			next_process->sleep_time=1000;
		}
		else if (next_process->sleep_time<0)
		{
			next_process->sleep_time=0;
		}

		if (next_process->assigned_sleep_time==0)
		{		
			adjust_sched_queue(next->val);			
			next_process->assigned_sleep_time=0;
			next_process->proc_status=RUNNING;
			queue_index=next_process->curr_sched_queue_index;
			ll_append(system.scheduler_desc->scheduler_queue[queue_index],next_process);
			old_node=next;
			next=ll_next(next);
			ll_delete_node(old_node);
			is_schedule=1;
		}
		else 
		{
			next=ll_next(next);
		}
		next_process=next->val;
	}

	if (sleeping_process!=NULL && !system.active_console_desc->is_empty)
	{
		_awake(sleeping_process);
		system.active_console_desc->sleeping_process=NULL;
	}
	else
	{	
		process_context=system.process_info->current_process->val;
		process_context->sleep_time-=QUANTUM_DURATION;
		if (process_context->sleep_time>1000) 	
		{
			process_context->sleep_time=1000;
		}
		else if (process_context->sleep_time<0)
		{
			process_context->sleep_time=0;
		}

		if (process_context->proc_status==EXITING)
		{
			is_schedule=2;
		}
		else 
		{
			process_context->tick--;
			if (process_context->tick==0) 
			{
				process_context->tick=TICK;
				is_schedule=1;	
			}
		}
	}

	//MANAGE TIMERS
exit_handler:;
	sentinel_node = ll_sentinel(system.timer_list);
	node = ll_first(system.timer_list);
	first_node = node;
	//while(node != sentinel_node)
	do
	{
		timer = node->val;
		timer->val --;
		if (timer->val <= 0 )
		{
			(*timer->handler)(timer->handler_arg);
			break;
		}
		node = ll_next(node);
	}
	while(node != ll_first(system.timer_list));
	EXIT_INT_HANDLER(is_schedule,processor_reg);
}
*/
