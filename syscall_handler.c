#include "general.h"
#include "system.h"
#include "asm.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "drivers/ata/ata.h"
#include "syscall_handler.h"

#define K_STACK 0x1FFFFB

extern t_system system;

void stop()
{
	int xx;
	xx++;
}

void syscall_handler()
{
	int xx=0;
	static temp_sys_num;
	static int free_vm_proc;        
	int syscall_num;
	unsigned int mem_size;
	static struct t_process_context* new_process_context;
	static struct t_process_context* current_process_context;
	static struct t_process_context* old_process_context;
	static struct t_processor_reg processor_reg;
	int* params;
	char data;
 	
	CLI
	SAVE_PROCESSOR_REG
	SWITCH_DS_TO_KERNEL_MODE
	if (*(int*)K_STACK!=0x23)
	{
		stop();
	}	
	system.race_tracker.buffer[system.race_tracker.index++]=0;
	free_vm_proc=0;
	current_process_context=system.process_info.current_process->val;
	old_process_context=current_process_context;
	t_console_desc *console_desc=current_process_context->console_desc;
	syscall_num=processor_reg.eax;
	temp_sys_num=syscall_num;
	params=processor_reg.ecx;
	if (syscall_num==1) 
	{
		params[0]=_fork(processor_reg);
		goto exit_1;
	}
	else if (syscall_num==2)
	{ 
		params[1]=_malloc(params[0]);
		goto exit_1;
	}
	else if (syscall_num==3)
	{ 
		_free(params[0]);
		goto exit_1;
	}
	else if (syscall_num==4)
	{ 
		_write_char(console_desc,params[0]);
		goto exit_1;
	}
	else if (syscall_num==5)
	{ 
		data=_read_char(console_desc);
		*((char*)params[0])=data;	
		if (data==NULL)
		{
			goto exit_2; 
		}
		goto exit_1;
	}
	else if (syscall_num==6)
	{ 
		_echo_char(console_desc,params[0]);
		goto exit_1;	
	}
	else if (syscall_num==7)
	{ 
		_enable_cursor(console_desc);
		goto exit_1;	
	}
	else if (syscall_num==8)
	{ 
		_disable_cursor(console_desc);
		goto exit_1;	
	}
	else if (syscall_num==9)
	{ 
		_update_cursor(console_desc);
		goto exit_1;		
	}
	else if (syscall_num==10)
	{
		_delete_char(console_desc);
		goto exit_1;	 
	}
	else if (syscall_num==11)
	{
		_pause();	
		goto exit_2; 
	}
	else if (syscall_num==12)
	{
		//_awake(params[0],params[1]);
		_awake(params[0]);
		goto exit_1; 
	}
	else if (syscall_num==13)
	{
		_exit(params[0]);
		free_vm_proc=1;
		goto exit_2; 
	}
	else if (syscall_num==14) 
	{
		_exec(params[0],params[1]);
		//dead code point 
	}
	else if (syscall_num==15) 
	{
		_sleep_time(params[0]);	
		goto exit_2; 
	}
//	else if (syscall_num==16) 
//	{
//		_read_28_ata(params[0],params[1],params[2],params[3]);
//		goto exit_2; 
//	}
//	else if (syscall_num==17) 
//	{
//		_write_28_ata(params[1],params[0],params[2]);
//		goto exit_2; 
//	}
	else if (syscall_num==18) 
	{
		params[3]=_open(system.root_fs,(char*) params[0],params[1],params[2]); 
		goto exit_2; 
	}

	else if (syscall_num==19) 
	{
		params[2]=_close(system.root_fs,params[0],params[1]);
		goto exit_2; 
	}

	else if (syscall_num==20) 
	{
		params[4]=_read(system.root_fs,(void*)params[0],params[1],params[2],params[3]); 
		goto exit_2; 
	}

	else if (syscall_num==21) 
	{
		params[4]=_write(system.root_fs,(void*)params[0],params[1],params[2],params[3]);
		goto exit_2; 
	}
	
	else if (syscall_num==22)
	{
		params[2]=_rm(system.root_fs,(char*)params[0],params[1]);
		goto exit_2; 
	}

	else if (syscall_num==23) 
	{
		params[2]=_mkdir(system.root_fs,params[0],params[1]);
		goto exit_2; 
	}
exit_1:
	system.race_tracker.buffer[system.race_tracker.index++]=2;
	if (*(int*)K_STACK!=0x23)
	{
		stop();
	}
        SWITCH_DS_TO_USER_MODE
	RESTORE_PROCESSOR_REG
	RET_FROM_INT_HANDLER
////USED SEMICOLON WORKAROUND TO AVOID "a label can only be part of a statement and a declaration is not a statement" ERROR.
//// A SEMICOLON DEFINE A EMPTY STATEMENT.
//exit_2:;
//	//NOT USED SAVE_IF_STATUS BECAUSE PAGE SWITCH
//	CLI
//	current_process_context=system.process_info.current_process->val;
//	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir)))
//	if (free_vm_proc) 
//	{
//		DO_STACK_FRAME(processor_reg.esp-8);
//		free_vm_process(old_process_context->page_dir);
//	}
//	SWITCH_DS_TO_USER_MODE
//	RESTORE_PROCESSOR_REG
//	STI
//	EXIT_SYSCALL_HANDLER

//USED SEMICOLON WORKAROUND TO AVOID "a label can only be part of a statement and a declaration is not a statement" ERROR.
// A SEMICOLON DEFINE A EMPTY STATEMENT.
exit_2:;
	//NOT USED SAVE_IF_STATUS BECAUSE PAGE SWITCH
//	CLI
	schedule(current_process_context,&processor_reg);
	system.race_tracker.buffer[system.race_tracker.index++]=1;
	if (*(int*)K_STACK!=0x23)
	{
		stop();
	}
	new_process_context=system.process_info.current_process->val;
	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) new_process_context->page_dir)))
	if (free_vm_proc) 
	{
		DO_STACK_FRAME(processor_reg.esp-8);
		free_vm_process(old_process_context->page_dir);
	}
	system.race_tracker.buffer[system.race_tracker.index++]=temp_sys_num;
	system.race_tracker.buffer[system.race_tracker.index++]=2;
	if (*(int*)K_STACK!=0x23)
	{
		stop();
	}
	SWITCH_DS_TO_USER_MODE
	RESTORE_PROCESSOR_REG
//	STI
	EXIT_SYSCALL_HANDLER
}
