#include "timer.h"
#include "asm.h"  
#include "idt.h" 
#include "virtual_memory/vm.h"
#include "drivers/lapic/lapic.h"
#include "drivers/pit/8253.h" 

static u32 interrupt_counter = 0;

extern int counter;

static void init_timer();
static void int_handler_lapic();
static u32 read_reg(u32 reg_offset);
static void write_reg(u32 reg_offset, u32 val);
static void set_timer_divisor(int divisor);

void enable_irq_line()
{
        //FAKE JUST TO COMPILE
}

void disable_irq_line()
{
        //FAKE JUST TO COMPILE
}

void init_lapic()
{
	u32 val;
	u32 lapic_id;
	
	map_vm_mem(system.master_page_dir, LAPIC_BASE, LAPIC_BASE, PAGE_SIZE,3);
	init_pit();
	// Clear Task Priority register; this enables all LAPIC interrupts
	val = read_reg(LAPIC_TPR);
	val &= ~0xff;
	write_reg(LAPIC_TPR, val);
	// Enable the LAPIC
	val = read_reg(LAPIC_SVR);
    val |= LAPIC_SVR_APIC_EN;
	write_reg(LAPIC_SVR, val);
	lapic_id = read_reg(LAPIC_ID);
	STI
	init_timer();
	CLI
	free_pit();
}

static void init_timer()
{
	static struct t_i_desc i_desc;
	t_timer* timer = NULL;
	u32 lapic_freq;
	u32 max_timer_count = 0xffffffff;
	u32 count;
	u32 divisor;

	timer = timer_init(0, NULL, NULL, NULL);
	for (divisor = 2; divisor <= 128; divisor *= 2)
	{
		set_timer_divisor(divisor);
		write_reg(LAPIC_TMR, LAPIC_TMR_ONESHOT | LAPIC_TMR_M);
		write_reg(LAPIC_ICR, max_timer_count);
		timer_set(timer, ONE_SEC_DELAY);
		while (timer->val > 0 );
		lapic_freq = max_timer_count - read_reg(LAPIC_CCR);
		if (lapic_freq != max_timer_count) 
		{
			break;
		}
	}
	if (divisor > 128)
	{
		panic();
	}

	i_desc.baseLow = ((int) &int_handler_lapic) & 0xFFFF;
	i_desc.selector = 0x8;
	i_desc.flags = 0x08e00;
	i_desc.baseHi =((int) &int_handler_lapic) >> 0x10;
	set_idt_entry(0x38, &i_desc);

	count = lapic_freq / TICK_FRQ; //lapic_freq / count = TICK_FRQ
	write_reg(LAPIC_TMR, LAPIC_TMR_PERIODIC | LAPIC_TMR_VECTOR);
	write_reg(LAPIC_ICR, count);
}

static void set_timer_divisor(int divisor)
{
	unsigned int dv;
	switch (divisor) 
	{
     	case 1:
     		dv = LAPIC_LVT_DV_1;
			break;
       	case 2:
       		dv = LAPIC_LVT_DV_2;
			break;
       	case 4:
      		dv = LAPIC_LVT_DV_4;
			break;
       	case 8:
       		dv = LAPIC_LVT_DV_8;
			break;
      	case 16:
     		dv = LAPIC_LVT_DV_16;
			break;
      	case 32:
     		dv = LAPIC_LVT_DV_32;
			break;
      	case 64:
     		dv = LAPIC_LVT_DV_64;
			break;
    	case 128:
     		dv = LAPIC_LVT_DV_128;
			break;
   		default:
      		panic();
			break;
	}
	write_reg(LAPIC_DCR, dv);
}

static u32 read_reg(u32 reg_offset)
{
	unsigned char* address = NULL;
	u32 val;

	address = LAPIC_BASE + reg_offset;
	val = *((volatile u32*)(address));
	return val;
}

static void write_reg(u32 reg_offset, u32 val)
{
	unsigned char* address = NULL;

	address = LAPIC_BASE + reg_offset;
	(*((volatile u32*)(address))) = (val);
}

void int_handler_lapic()
{
	int is_schedule = 0;
	struct t_process_context* process_context;
	struct t_process_context* sleeping_process;
	struct t_processor_reg processor_reg;
	t_llist_node* next = NULL;
	t_llist_node* sentinel = NULL;
	t_llist_node* old_node = NULL;
	struct t_process_context* next_process = NULL;
	unsigned int queue_index;
	unsigned int priority;
	t_llist_node* sentinel_node = NULL;
	t_llist_node* node = NULL;
	t_llist_node* first_node = NULL;
	t_timer* timer = NULL;
	
	SAVE_PROCESSOR_REG
	EOI_TO_LAPIC
	SWITCH_DS_TO_KERNEL_MODE
	process_context = system.process_info->current_process->val;

	system.time += QUANTUM_DURATION;
	if (system.int_path_count > 0)
	{
		goto EXIT_HANDLER;
	}
	sleeping_process = system.active_console_desc->sleeping_process;
	sentinel = ll_sentinel(system.process_info->sleep_wait_queue);
	next = ll_first(system.process_info->sleep_wait_queue);
	next_process = next->val;
	//THIS STUFF MUST BE MOVED INSIDE ASSIGNED SLEEP MANAGER LIKE IO
	while(next != sentinel)
	{
		next_process->assigned_sleep_time -= QUANTUM_DURATION;
		next_process->sleep_time += QUANTUM_DURATION;
		
		if (next_process->sleep_time > 1000) 	
		{
			next_process->sleep_time = 1000;
		}
		else if (next_process->sleep_time < 0)
		{
			next_process->sleep_time = 0;
		}
		if (next_process->assigned_sleep_time == 0)
		{		
			adjust_sched_queue(next->val);			
			next_process->assigned_sleep_time = 0;
			next_process->proc_status = RUNNING;
			queue_index = next_process->curr_sched_queue_index;
			ll_append(system.scheduler_desc->scheduler_queue[queue_index], next_process);
			old_node = next;
			next=ll_next(next);
			ll_delete_node(old_node);
			is_schedule = 1;
		}
		else 
		{
			next=ll_next(next);
		}
		next_process = next->val;
	}

	if (sleeping_process != NULL && !system.active_console_desc->is_empty)
	{
		_awake(sleeping_process);
		system.active_console_desc->sleeping_process = NULL;
	}
	else
	{	
		process_context = system.process_info->current_process->val;
		process_context->sleep_time -= QUANTUM_DURATION;
		if (process_context->sleep_time > 1000) 	
		{
			process_context->sleep_time = 1000;
		}
		else if (process_context->sleep_time < 0)
		{
			process_context->sleep_time = 0;
		}

		if (process_context->proc_status == EXITING)
		{
			is_schedule = 2;
		}
		else 
		{
			process_context->tick--;
			if (process_context->tick == 0) 
			{
				process_context->tick = TICK;
				is_schedule = 1;	
			}
		}
	}

	//MANAGE TIMERS
EXIT_HANDLER:;
	sentinel_node = ll_sentinel(system.timer_list);
	node = ll_first(system.timer_list);
	first_node = node;
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

/*
static void __int_handler_lapic()
{
	struct t_processor_reg processor_reg;
	struct t_processor_reg _processor_reg;

	SAVE_PROCESSOR_REG
	SWITCH_DS_TO_KERNEL_MODE
	
	if (interrupt_counter == 0)
	{
			interrupt_counter = 0;
	}

	interrupt_counter++;
	if(interrupt_counter == 1000)
	{
		interrupt_counter = 0;
	}
	_processor_reg = processor_reg;
	EOI_TO_LAPIC
	RESTORE_PROCESSOR_REG                                                                                   
	RET_FROM_INT_HANDLER
}
*/
