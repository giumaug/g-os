#include "drivers/pit/8253.h" 
#include "asm.h"  
#include "idt.h" 
#include "timer.h" 
#include "virtual_memory/vm.h"
#include "drivers/pic/8259A.h" 

#define K_STACK 0x1FFFFB

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
	set_idt_entry(0x20,&i_desc);
}

void int_handler_pit()
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

//exit_handler:;
	EXIT_INT_HANDLER(is_schedule,processor_reg);

/*
	static struct t_process_context _current_process_context;                                                  	
	static struct t_process_context _old_process_context;                                                      	
	static struct t_process_context _new_process_context;	                                                        
	static struct t_processor_reg _processor_reg;                                                                   
	static unsigned int _action2; 
	static u8 stop = 0;                                                                              
                                                                                                                   
	CLI                                                                                                             
	if (system.int_path_count == 0 && system.force_scheduling == 0 && system.flush_network == 1)                    
	{                                                                                                               
                        system.flush_network = 0;                                                                       
			dequeue_packet(system.network_desc);                                                            
			equeue_packet(system.network_desc);                                                             
                        system.flush_network = 1;                                                                       
	}                                                                                                               
	_action2=is_schedule;                                                                                      
	_current_process_context=*(struct t_process_context*)system.process_info->current_process->val;                 
	_old_process_context=_current_process_context;                                                                  
	_processor_reg=processor_reg;                                                                                   
	if (system.force_scheduling == 1 && 0 == 0 && system.int_path_count == 0)                                  
	{                                                                                                               
		_action2 = 1;                                                                                           
		if (_current_process_context.proc_status == EXITING)                                                    
		{                                                                                                       
			_action2 = 2;                                                                                   
		}                                                                                                       
	}                                                                                                               
                                                                                                                        
	if (_action2>0)                                                                                                 
	{	system.force_scheduling = 0;
		stop = 0;                                                                            
		while(!stop)                                                                                             
		{                                                                                                       
			schedule(&_current_process_context,&_processor_reg);                                            
			_new_process_context = *(struct t_process_context*) system.process_info->current_process->val;  
			if (_new_process_context.sig_num == SIGINT)                                                    
			{                                                                                            
				_exit(0);
				free_vm_process(&_new_process_context);                                                         
				buddy_free_page(system.buddy_desc,FROM_PHY_TO_VIRT(_new_process_context.phy_kernel_stack));           
			}                                                                                               
			else                                                                                            
			{                                                                                               
				stop = 1;                                                                               
			}                                                                                               
		}     
		if (_new_process_context.pid != _old_process_context.pid)                                               
		{                                                                                                       
				_processor_reg=_new_process_context.processor_reg;                                      
		}                                                                                                       
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) _new_process_context.page_dir)))                       
		DO_STACK_FRAME(_processor_reg.esp-8);                                                                   
                                                                                                                        
		if (_action2==2)                                                                                        
		{                                                                                                       
			DO_STACK_FRAME(_processor_reg.esp-8);                                                           
			free_vm_process(&_old_process_context);                                                         
			buddy_free_page(system.buddy_desc,FROM_PHY_TO_VIRT(_old_process_context.phy_kernel_stack));     
		}                                                                                                       
		RESTORE_PROCESSOR_REG                                                                                   
		EXIT_SYSCALL_HANDLER                                                                                    
	}                                                                                                          	
	else                                                                                                       	
	{                                                                                                               
		RESTORE_PROCESSOR_REG                                                                                   
		RET_FROM_INT_HANDLER                                                                                    
	}
*/
}
