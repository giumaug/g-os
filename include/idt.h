#ifndef IDT_H                
#define IDT_H

#include "scheduler/process.h"

struct t_i_desc {
   unsigned short int baseLow;    	 
   unsigned short int selector;      	
   unsigned short flags;
   unsigned short baseHi;
};

struct t_idt_ptr {
	unsigned short int idt_size __attribute__((__packed__));
	unsigned int idt_address __attribute__((__packed__));
};

void int_handler_generic();
void init_idt();
void set_idt_entry(int entry,struct t_i_desc* i_desc);
void exit_int_handler(unsigned int action,struct t_processor_reg processor_reg,short ds);

#define EXIT_INT_HANDLER(action,processor_reg,ds)                                                           \
{                                                                                                           \
	static struct t_process_context* new_process_context;                                               \
	static struct t_process_context* current_process_context;                                           \
	static struct t_process_context* old_process_context;                                               \
                                                                                                            \
	CLI                                                                                                 \
	current_process_context=system.process_info.current_process->val;                                   \
	old_process_context=current_process_context;                                                        \
	if (action>0)                                                                                       \
	{                                                                                                   \
		schedule(current_process_context,&processor_reg);                                           \
		new_process_context=system.process_info.current_process->val;                               \
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) new_process_context->page_dir)))           \
		DO_STACK_FRAME(processor_reg.esp-8);                                                        \
		if (action==2)                                                                              \
		{                                                                                           \
			DO_STACK_FRAME(processor_reg.esp-8);                                                \
			free_vm_process(old_process_context->page_dir);                                     \
		}                                                                                           \
		SWITCH_DS_TO_USER_MODE                                                                      \
		RESTORE_PROCESSOR_REG                                                                       \
		EXIT_SYSCALL_HANDLER                                                                        \
	}                                                                                                   \
	else                                                                                                \
	{                                                                                                   \
		if (ds==0x20)                                                                               \
		{                                                                                           \
			SWITCH_DS_TO_USER_MODE                                                              \
		}                                                                                           \
		RESTORE_PROCESSOR_REG                                                                       \
		RET_FROM_INT_HANDLER                                                                        \
	}                                                                                                   \
}      

#endif

