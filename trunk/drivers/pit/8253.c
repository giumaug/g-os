#include "general.h"
#include "asm.h"  
#include "idt.h" 
#include "drivers/pit/8253.h" 
#include "system.h"  
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "drivers/pic/8259A.h" 

extern t_system system;

void init_pit()
{	
	static struct t_i_desc i_desc;
	out(BINARIY_COUNT_MODE,CONTROL_WORD);
	out(LOW_QUANTUM_DURATION,COUNTER_0);
	out(HI_QUANTUM_DURATION,COUNTER_0);
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
	static struct t_processor_reg processor_reg;
	
	SAVE_PROCESSOR_REG
	EOI
	disable_irq_line(0);
	STI
	GET_DS(ds)
	if (ds==0x20) 
	{
		SWITCH_DS_TO_KERNEL_MODE
	}
	sleeping_process=system.active_console_desc->sleeping_process;
	if (sleeping_process!=NULL && !system.active_console_desc->is_empty)
	{
		_awake(sleeping_process,&processor_reg);
		system.active_console_desc->sleeping_process=NULL;
		is_schedule=1;
	}
	else
	{
		process_context=system.process_info.current_process->val;
		process_context->tick--;
		if (process_context->tick==0) 
		{
			process_context->tick=TICK;
			schedule(&processor_reg);
			is_schedule=1;	
		}
	}
	CLI
	enable_irq_line(0);
	if (is_schedule==1) {
		//NOW PROCESS_CONTEXT IS NEW PROCESS TO SCHEDULE!!!!!
		process_context=system.process_info.current_process->val;
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) process_context->page_dir)))
		asm ("movl %0,%%esp;"::"r"(processor_reg.esp)); 
		asm ("movl %0,%%eax;"::"r"(processor_reg.eax)); 
		asm ("push %eax;");                             
		asm ("movl %0,%%ebx;"::"r"(processor_reg.ebx)); 
		asm ("push %ebx;");                             
		asm ("movl %0,%%ecx;"::"r"(processor_reg.ecx)); 
		asm ("push %ecx;");                             
		asm ("movl %0,%%edx;"::"r"(processor_reg.edx)); 
		asm ("push %edx;");                             
		asm ("movl %0,%%esi;"::"r"(processor_reg.esi)); 
		asm ("push %esi;");                             
		asm ("movl %0,%%edi;"::"r"(processor_reg.edi)); 
		asm ("push %edi;");                             
		asm ("pop %edi;");                              
		asm ("pop %esi;");                              
		asm ("pop %edx;");                              
		asm ("pop %ecx;");                              
		asm ("pop %ebx;");                              
		asm ("pop %eax;");
		//---RESTORE_PROCESSOR_REG
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

