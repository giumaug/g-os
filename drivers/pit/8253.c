#include "general.h"
#include "asm.h"  
#include "idt.h" 
#include "system.h"  
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "drivers/pic/8259A.h" 
#include "drivers/pit/8253.h" 

extern t_system system;

void init_pit()
{	
	static struct t_i_desc i_desc;
	out(BINARIY_COUNT_MODE,CONTROL_WORD);
	out(LOW_QUANTUM,COUNTER_0);
	out(HI_QUANTUM,COUNTER_0);
	i_desc.baseLow=((int)&int_handler_pit) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00;
	i_desc.baseHi=((int)&int_handler_pit)>>0x10;
	set_idt_entry(0x20,&i_desc);
}

void int_handler_pit()
{	
	int is_schedule=0;
	short ds;
	struct t_process_context* process_context;
	struct t_process_context* sleeping_process;
	struct t_process_context* new_process_context;
	static struct t_processor_reg processor_reg;
	t_llist_node* next;
	t_llist_node* sentinel;
	t_llist_node* old_node;
	struct t_process_context* next_process;
	unsigned int queue_index;
	unsigned int priority;
	
	SAVE_PROCESSOR_REG
	EOI
//	disable_irq_line(0);
//	STI
	CLI
	GET_DS(ds)
	if (ds==0x20) 
	{
		SWITCH_DS_TO_KERNEL_MODE
	}
	sleeping_process=system.active_console_desc->sleeping_process;
	
	sentinel=ll_sentinel(system.sleep_wait_queue);
	next=ll_first(system.sleep_wait_queue);
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
			ll_append(system.scheduler_desc.scheduler_queue[queue_index],next_process);
			old_node=next;
			next=ll_next(next);
			ll_delete_node(old_node);
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
		if (sleeping_process!=NULL)
		{
			sleeping_process->sleep_time+=QUANTUM_DURATION;
			if (sleeping_process->sleep_time>1000) 	
			{
				sleeping_process->sleep_time=1000;
			}
			else if (sleeping_process->sleep_time<0)
			{		
				sleeping_process->sleep_time=0;
			}
		}
		process_context=system.process_info.current_process->val;
		process_context->sleep_time-=QUANTUM_DURATION;
		
		if (process_context->sleep_time>1000) 	
		{
			process_context->sleep_time=1000;
		}
		else if (process_context->sleep_time<0)
		{
			process_context->sleep_time=0;
		}

		process_context->tick--;
		if (process_context->tick==0) 
		{
			process_context->tick=TICK;
			//schedule();
			is_schedule=1;	
		}
	}
//	CLI
//	enable_irq_line(0);
	if (is_schedule==1) {
		process_context=system.process_info.current_process->val;
		schedule(process_context,&processor_reg);
		new_process_context=system.process_info.current_process->val;
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) new_process_context->page_dir)))
		RESTORE_PROCESSOR_REG
		SWITCH_DS_TO_USER_MODE
		EXIT_SYSCALL_HANDLER
 	}
	else {
		if (ds==0x20) 
		{
			SWITCH_DS_TO_USER_MODE
		}
		RESTORE_PROCESSOR_REG
		RET_FROM_INT_HANDLER
	}
}

