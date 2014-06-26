#include "general.h"
#include "system.h"
#include "asm.h"
#include "idt.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "drivers/ata/ata.h"
#include "syscall_handler.h"
#include "debug.h"

#define K_STACK 0x1FFFFB

extern t_system system;

unsigned int counter=0;

void syscall_handler()
{
	static int free_vm_proc;        
	int syscall_num;
	unsigned int mem_size;
	struct t_process_context* current_process_context;
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
	}
	else if (syscall_num==2)
	{ 
		params[1]=_malloc(params[0]);
	}
	else if (syscall_num==3)
	{ 
		_free(params[0]);
	}
	else if (syscall_num==150)
	{ 
		params[1]=_bigMalloc(params[0]);
	}
	else if (syscall_num==151)
	{ 
		_bigFree(params[0]);
	}
	else if (syscall_num==4)
	{ 
		_write_char(console_desc,params[0]);
	}
	else if (syscall_num==5)
	{ 
		data=_read_char(console_desc);
		*((char*)params[0])=data;	
		if (data==NULL)
		{
			on_exit_action=1; 
		}
	}
	else if (syscall_num==6)
	{ 
		_echo_char(console_desc,params[0]);
	}
	else if (syscall_num==7)
	{ 
		_enable_cursor(console_desc);	
	}
	else if (syscall_num==8)
	{ 
		_disable_cursor(console_desc);
	}
	else if (syscall_num==9)
	{ 
		_update_cursor(console_desc);	
	}
	else if (syscall_num==10)
	{
		_delete_char(console_desc);
	}
	else if (syscall_num==11)
	{
		_pause();	
		on_exit_action=1; 
	}
	else if (syscall_num==12)
	{
		_awake(params[0]);
	}
	else if (syscall_num==13)
	{
		_exit(params[0]);
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
		on_exit_action=1; 
	}
	else if (syscall_num==18) 
	{
		params[3]=_open(system.root_fs,(char*) params[0],params[1],params[2]); 
		on_exit_action=1; 
	}

	else if (syscall_num==19) 
	{
		params[2]=_close(system.root_fs,params[0],params[1]);
		on_exit_action=1; 
	}

	else if (syscall_num==20) 
	{
		char* xx=params[1];
		params[3]=_read(system.root_fs,params[0],params[1],params[2]); 
		on_exit_action=1; 
	}

	else if (syscall_num==21) 
	{
		params[4]=_write(system.root_fs,(void*)params[0],params[1],params[2],params[3]);
		on_exit_action=1;  
	}
	
	else if (syscall_num==22)
	{
		params[2]=_rm(system.root_fs,(char*)params[0],params[1]);
		on_exit_action=1; 
	}

	else if (syscall_num==23) 
	{
		params[2]=_mkdir(system.root_fs,params[0],params[1]);
		on_exit_action=1; 
	}
	//syscall 24 and 25 test only
	else if (syscall_num==24) 
	{
		t_io_request* io_request;
		io_request=kmalloc(sizeof(t_io_request));
		io_request->device_desc=system.device_desc;
		io_request->sector_count=params[0];
		io_request->lba=params[1];
		io_request->io_buffer=params[2];
		io_request->process_context=current_process_context;
		_read_28_ata(io_request);
		kfree(io_request);
	}
	else if (syscall_num==25) 
	{
		t_io_request* io_request;
		io_request=kmalloc(sizeof(t_io_request));
		io_request->device_desc=system.device_desc;
		io_request->sector_count=params[0];
		io_request->lba=params[1];
		io_request->io_buffer=params[2];
		io_request->process_context=current_process_context;
		_write_28_ata(io_request);
		kfree(io_request);
	}
	else if (syscall_num==101) 
	{
		on_exit_action=1; 
	}
	else if (syscall_num==102) 
	{
//		params[0]=_flush_ata_pending_request();
		_flush_ata_pending_request();
	}
	else
	{
		panic();
	}
	
	
	
//	EXIT_INT_HANDLER(on_exit_action,processor_reg,0)

	static struct t_process_context _current_process_context;                                          
	static struct t_process_context _old_process_context;                                              
	static struct t_process_context _new_process_context;	                                            
	static struct t_processor_reg _processor_reg;                                                       
	static unsigned int _action;                                                                        
                                                                                                            
	CLI                                                                         
	_action=on_exit_action;                                                                                
	_current_process_context=*(struct t_process_context*)system.process_info.current_process->val;                                  
	_old_process_context=_current_process_context;                                                      
	_processor_reg=processor_reg;                                                               
	if (_action>0)                                                                                      
	{                                                                                   
		schedule(&_current_process_context,&_processor_reg);                            
		_new_process_context=*(struct t_process_context*)(system.process_info.current_process->val);                              
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) _new_process_context.page_dir)))                                                          
		DO_STACK_FRAME(_processor_reg.esp-8); 
		if (_action==2)                                                                              
		{                                                                           
			DO_STACK_FRAME(_processor_reg.esp-8);                                               
			free_vm_process(_old_process_context.page_dir); 
			buddy_free_page(&system.buddy_desc,FROM_PHY_TO_VIRT(_old_process_context.phy_add_space));                                
		}                                                                               
		SWITCH_DS_TO_USER_MODE                                                                      
		RESTORE_PROCESSOR_REG                                                                       
		EXIT_SYSCALL_HANDLER                                                                        
	}                                                                                                   
	else                                                                                                
	{   
		DO_STACK_FRAME(_processor_reg.esp-8);                                                                                            
		if (1)                                                                               
		{                                                                                           
			SWITCH_DS_TO_USER_MODE                                                              
		}                                                                                           
		RESTORE_PROCESSOR_REG                                                                       
		RET_FROM_INT_HANDLER                                                                        
	}
}
