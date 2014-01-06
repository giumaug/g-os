#ifndef IDT_H                
#define IDT_H

#include "scheduler/process.h"

extern unsigned int exit_count_1;
extern unsigned int exit_count_2;
extern unsigned int exit_count_3;
extern unsigned int exit_count_4;
extern unsigned int exit_count_5;
extern unsigned int exit_count_6;

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

#define EXIT_INT_HANDLER(action,processor_reg,ds)                                                                  \
                                                                                                                   \
	static struct t_process_context _current_process_context;                                                  \
	static struct t_process_context _old_process_context;                                                      \
	static struct t_process_context _new_process_context;	                                                   \
	static struct t_processor_reg _processor_reg;                                                              \
	static unsigned int _action2;                                                                              \
                                                                                                                   \
	CLI                                                                                                        \
	_action2=action;                                                                                           \
	_current_process_context=*(struct t_process_context*)system.process_info.current_process->val;             \
	_old_process_context=_current_process_context;                                                             \
	_processor_reg=processor_reg;                                                                              \
	if (_action2>0)                                                                                            \
	{                                                                                                          \
		schedule(&_current_process_context,&_processor_reg);                                               \
		_new_process_context=*(struct t_process_context*)system.process_info.current_process->val;         \
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) _new_process_context.page_dir)))                  \
		if (_action2==2)                                                                                   \
		{                                                                                                  \
			exit_count_2++;                                                                            \
			DO_STACK_FRAME(_processor_reg.esp-8);                                                      \
			free_vm_process(_old_process_context.page_dir);                                            \
			buddy_free_page(&system.buddy_desc,FROM_PHY_TO_VIRT(_old_process_context.phy_add_space));  \
		}                                                                                                  \
		SWITCH_DS_TO_USER_MODE                                                                             \
		RESTORE_PROCESSOR_REG                                                                              \
		EXIT_SYSCALL_HANDLER                                                                               \
	}                                                                                                          \
	else                                                                                                       \
	{                                                                                                          \
		if (ds==0x20)                                                                                      \
		{                                                                                                  \
			SWITCH_DS_TO_USER_MODE                                                                     \
		}                                                                                                  \
		RESTORE_PROCESSOR_REG                                                                              \
		RET_FROM_INT_HANDLER                                                                               \
	}
                                                                                                           

#endif

