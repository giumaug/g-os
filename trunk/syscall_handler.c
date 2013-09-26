#include "general.h"
#include "system.h"
#include "asm.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "drivers/ata/ata.h"
#include "syscall_handler.h"

#define K_STACK 0x1FFFFB

extern t_system system;

void syscall_handler()
{
	static int free_vm_proc;        
	int syscall_num;
	unsigned int mem_size;
	struct t_process_context* new_process_context;
	struct t_process_context* current_process_context;
	struct t_process_context* old_process_context;
	struct t_processor_reg processor_reg;
	int* params;
	char data;
	unsigned int on_exit_action;
 	
	SAVE_PROCESSOR_REG
	SWITCH_DS_TO_KERNEL_MODE
	on_exit_action=0;
	current_process_context=system.process_info.current_process->val;
	t_console_desc *console_desc=current_process_context->console_desc;
	syscall_num=processor_reg.eax;
	params=processor_reg.ecx;
	if (syscall_num==1) 
	{
		params[0]=_fork(processor_reg);
//		goto exit_1;
	}
	else if (syscall_num==2)
	{ 
		params[1]=_malloc(params[0]);
//		goto exit_1;
	}
	else if (syscall_num==3)
	{ 
		_free(params[0]);
//		goto exit_1;
	}
	else if (syscall_num==4)
	{ 
		_write_char(console_desc,params[0]);
//		goto exit_1;
	}
	else if (syscall_num==5)
	{ 
		data=_read_char(console_desc);
		*((char*)params[0])=data;	
		if (data==NULL)
		{
//			goto exit_2;
			on_exit_action=1; 
		}
//		goto exit_1;
	}
	else if (syscall_num==6)
	{ 
		_echo_char(console_desc,params[0]);
//		goto exit_1;	
	}
	else if (syscall_num==7)
	{ 
		_enable_cursor(console_desc);
//		goto exit_1;	
	}
	else if (syscall_num==8)
	{ 
		_disable_cursor(console_desc);
//		goto exit_1;	
	}
	else if (syscall_num==9)
	{ 
		_update_cursor(console_desc);
//		goto exit_1;		
	}
	else if (syscall_num==10)
	{
		_delete_char(console_desc);
//		goto exit_1;	 
	}
	else if (syscall_num==11)
	{
		_pause();	
//		goto exit_2;
		on_exit_action=1; 
	}
	else if (syscall_num==12)
	{
		_awake(params[0]);
//		goto exit_1; 
	}
	else if (syscall_num==13)
	{
		_exit(params[0]);
//		free_vm_proc=1;
//		goto exit_2;
		on_exit_action=2; 
	}
	else if (syscall_num==14) 
	{
		_exec(params[0],params[1]);
		//dead code point 
	}
	else if (syscall_num==15) 
	{
		_sleep_time(params[0]);	
//		goto exit_2;
		on_exit_action=1; 
	}
	else if (syscall_num==18) 
	{
		params[3]=_open(system.root_fs,(char*) params[0],params[1],params[2]); 
//		goto exit_2;
		on_exit_action=1; 
	}

	else if (syscall_num==19) 
	{
		params[2]=_close(system.root_fs,params[0],params[1]);
//		goto exit_2;
		on_exit_action=1; 
	}

	else if (syscall_num==20) 
	{
		params[4]=_read(system.root_fs,(void*)params[0],params[1],params[2],params[3]); 
//		goto exit_2;
		on_exit_action=1; 
	}

	else if (syscall_num==21) 
	{
		params[4]=_write(system.root_fs,(void*)params[0],params[1],params[2],params[3]);
//		goto exit_2;
		on_exit_action=1;  
	}
	
	else if (syscall_num==22)
	{
		params[2]=_rm(system.root_fs,(char*)params[0],params[1]);
//		goto exit_2;
		on_exit_action=1; 
	}

	else if (syscall_num==23) 
	{
		params[2]=_mkdir(system.root_fs,params[0],params[1]);
//		goto exit_2;
		on_exit_action=1; 
	}
	exit_int_handler(on_exit_action,processor_reg,0);
//exit_1:
//        SWITCH_DS_TO_USER_MODE
//	RESTORE_PROCESSOR_REG
//	RET_FROM_INT_HANDLER
//
////USED SEMICOLON WORKAROUND TO AVOID "a label can only be part of a statement and a declaration is not a statement" ERROR.
//// A SEMICOLON DEFINE A EMPTY STATEMENT.
//exit_2:;
////	NOT USED SAVE_IF_STATUS BECAUSE PAGE SWITCH
////	CLI
//	schedule(current_process_context,&processor_reg);
//	new_process_context=system.process_info.current_process->val;
//	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) new_process_context->page_dir)))
//	DO_STACK_FRAME(processor_reg.esp-8);
//	if (free_vm_proc) 
//	{
//		DO_STACK_FRAME(processor_reg.esp-8);
//		check_stack_change();
//		free_vm_process(old_process_context->page_dir);
//		check_stack_change();
//	}
//	SWITCH_DS_TO_USER_MODE
//	RESTORE_PROCESSOR_REG
//	EXIT_SYSCALL_HANDLER
}
