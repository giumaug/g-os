#ifndef IDT_H                
#define IDT_H

#include "system.h"
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

#define EXIT_INT_HANDLER(action,processor_reg)                                                                          \
                                                                                                                        \
	static struct t_process_context _current_process_context;                                                  	\
	static struct t_process_context _old_process_context;                                                      	\
	static struct t_process_context _new_process_context;	                                                        \
	static struct t_processor_reg _processor_reg;                                                                   \
	static unsigned int _action2;                                                                                   \
        static u8 stop = 0;                                                                                             \
	static int ooo = -1;                                                                                            \
                                                                                                                        \
	ooo = -1;                                                                                                       \
	CLI                                                                                                             \
	if (system.int_path_count == 0 && system.force_scheduling == 0 && system.flush_network == 1)                    \
	{                                                                                                               \
                        system.flush_network = 0;                                                                       \
			dequeue_packet(system.network_desc);                                                            \
			equeue_packet(system.network_desc);                                                             \
                        system.flush_network = 1;                                                                       \
	}                                                                                                               \
	_action2=action;                                                                                                \
	_current_process_context=*(struct t_process_context*)system.process_info->current_process->val;                 \
	_old_process_context=_current_process_context;                                                                  \
	_processor_reg=processor_reg;                                                                                   \
	if (system.force_scheduling == 1 && action == 0 && system.int_path_count == 0)                                  \
	{                                                                                                               \
		_action2 = 1;                                                                                           \
		if (_current_process_context.proc_status == EXITING)                                                    \
		{                                                                                                       \
			_action2 = 2;                                                                                   \
		}                                                                                                       \
	}                                                                                                               \
                                                                                                                        \
	if (_action2>0)                                                                                                 \
	{	system.force_scheduling = 0;                                                                            \
                stop = 0;                                                                                               \
		while(!stop)                                                                                            \
		{                                                                                                       \
			schedule(&_current_process_context,&_processor_reg);                                            \
			_new_process_context = *(struct t_process_context*) system.process_info->current_process->val;  \
			if (_new_process_context.sig_num == SIGINT)                                                     \
			{                                                                                               \
                                if (ooo == _new_process_context.pid)                                                   \
				{                                                                                       \
					panic();                                                                        \
				}                                                                                       \
				_exit(0);                                                                               \
                                ooo = _new_process_context.pid;                                                         \
				free_vm_process(&_new_process_context);                                                      \
				buddy_free_page(system.buddy_desc,FROM_PHY_TO_VIRT(_new_process_context.phy_kernel_stack));  \
			}                                                                                               \
			else                                                                                            \
			{                                                                                               \
				stop = 1;                                                                               \
			}                                                                                               \
		}                                                                                                       \
		_check_process_context(ooo);                                                                            \
                                                                                                                        \
		/*_new_process_context=*(struct t_process_context*)system.process_info->current_process->val;*/         \
		if (_new_process_context.pid != _old_process_context.pid)                                               \
		{                                                                                                       \
				_processor_reg=_new_process_context.processor_reg;                                      \
		}                                                                                                       \
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) _new_process_context.page_dir)))                       \
		DO_STACK_FRAME(_processor_reg.esp-8);                                                                   \
                                                                                                                        \
		if (_action2==2)                                                                                        \
		{                                                                                                       \
			DO_STACK_FRAME(_processor_reg.esp-8);                                                           \
			free_vm_process(&_old_process_context);                                                         \
			buddy_free_page(system.buddy_desc,FROM_PHY_TO_VIRT(_old_process_context.phy_kernel_stack));     \
		}                                                                                                       \
		RESTORE_PROCESSOR_REG                                                                                   \
		EXIT_SYSCALL_HANDLER                                                                                    \
	}                                                                                                          	\
	else                                                                                                       	\
	{                                                                                                               \
		RESTORE_PROCESSOR_REG                                                                                   \
		RET_FROM_INT_HANDLER                                                                                    \
	}                                                                                                               \
         
#endif

