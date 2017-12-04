#include "asm.h"
#include "idt.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "drivers/ata/ata.h"
#include "syscall_handler.h"
#include "debug.h"

#define K_STACK 0x1FFFFB

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
	//call can come from kernel mode (sleep)
	SWITCH_DS_TO_KERNEL_MODE
	on_exit_action=0;
	current_process_context=system.process_info->current_process->val;
	t_console_desc *console_desc=current_process_context->console_desc;
	syscall_num=processor_reg.eax;
	params=processor_reg.ecx;

	switch (syscall_num) 
	{
		case 1:
		params[0]=_fork(processor_reg);
		break;
	
		case 2:
		params[1]=_malloc(params[0]);
		break;	

		case 3:
		_free(params[0]);
		break;
	
		case 150:
		params[1]=_bigMalloc(params[0]);
		break;
	
		case 151:
	 	_bigFree(params[0]);
		break;	

		case 4:
		_write_char(console_desc,params[0]);
		break;	

		case 5:
		data=_read_char(console_desc);
		*((char*)params[0])=data;	
		if (data==NULL)
		{
			on_exit_action=1; 
		}
		break;
	
		case 6:
		_echo_char(console_desc,params[0]);
		break;	

		case 7:
		_enable_cursor(console_desc);
		break;	
	
		case 8:
		_disable_cursor(console_desc);
		break;
	
		case 9:
		_update_cursor(console_desc);
		break;	
	
		case 10:
		_delete_char(console_desc);
		break;
	
		case 11:
		_pause();	
		on_exit_action=1;
		break; 
	
		case 12:
		_awake(params[0]);
		break;

		case 13:
		_exit(params[0]);
		on_exit_action=2;
		break;
	
		case 14: 
		params[2]=_exec(params[0],params[1]);
		break; 
	
		case 15:
		_sleep_time(params[0]);	
		on_exit_action=1;
		break; 
	
		case 18: 
		params[2]=_open(system.root_fs,(char*) params[0],params[1]); 
		on_exit_action=1;
		break; 

		case 19: 
		params[1]=_close(system.root_fs,params[0]);
		on_exit_action=1; 
		break;

		case 20: 
		params[3]=_read(system.root_fs,params[0],params[1],params[2]); 
		on_exit_action=1; 
		break;

		case 21: 
		params[3]=_write(system.root_fs,(void*)params[0],params[1],params[2]);
		on_exit_action=1;  
		break;

		case 36:
		params[3] = _seek(system.root_fs,params[0],params[1],params[2]);
		on_exit_action=1; 
		break;
	
		case 22:
		params[1]=_rm(system.root_fs,(char*)params[0]);
		on_exit_action=1; 
		break;

		case 23: 
		params[1]=_mkdir(system.root_fs,params[0]);
		on_exit_action=1; 
		break;

		//syscall 24 and 25 test only
		case 24:
		{ 
			//NEEDED NEW SCOPE TO DECLARE VARIABLE
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
		break;
	
		case 25:
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
		break;
	
		case 26: 
		params[1]=_chdir(system.root_fs,(char*) params[0]); 
		on_exit_action=1;
		break; 	
	
		case 27:
		params[2]=_stat(system.root_fs,(char*) params[0],params[1]); 	
		break;

		case 28:
 		params[1]=_open_socket(params[0]);
		break; 
	
		case 29:
 		params[5]=_bind(params[0],params[1],params[2],params[3],params[4]);
		break;	

		case 30:
 		params[5]=_recvfrom(params[0],params[1],params[2],params[3],params[4]);
		break;
	
		case 31:
 		params[5]=_sendto(params[0],params[1],params[2],params[3],params[4]);
		break;
	
		case 32:
 		params[1]=_close_socket(params[0]);
		break;
	
		case 33:
 		params[1]=_listen(params[0]);
		break;
	
		case 34:
 		params[3]=_accept(params[0]);
		break;
	
		case 35:
 		params[3]=_connect(params[0],params[1],params[2]);
		break;
	
		case 101: 
		on_exit_action=1;
		break;
	
		case 102:
		_flush_ata_pending_request();
		break;
	
		//DEBUG WRAPPER
		case 103:
		check_free_mem();
		break;
	
		case 104:
		debug_network(params[0],params[1]);
		break;
	
		case 105:
		_read_test(system.root_fs);
		break;
	
		default:
		panic();
	}
//	EXIT_INT_HANDLER(on_exit_action,processor_reg)

	CLI
	if (system.int_path_count == 0 && system.force_scheduling == 0)
	{
		equeue_packet(system.network_desc);
		dequeue_packet(system.network_desc);
	}
	if (system.force_scheduling == 1 && on_exit_action == 0 && system.int_path_count == 0)                                     
	{                                                                                                                       
		on_exit_action = 1;                                                                                             
	}
	
	static struct t_process_context _current_process_context;                                          
	static struct t_process_context _old_process_context;                                              
	static struct t_process_context _new_process_context;	                                            
	static struct t_processor_reg _processor_reg;                                                       
	static unsigned int _action;                                                                     
                                                                                                                                                                                
	_action=on_exit_action;                                                                                
	_current_process_context=*(struct t_process_context*)system.process_info->current_process->val;                                
	_old_process_context=_current_process_context;                                                      
	_processor_reg=processor_reg;                                                           
	if (_action>0)                                                                                      
	{  
		system.force_scheduling = 0;                                                                                 
		schedule(&_current_process_context,&_processor_reg);                            
		_new_process_context=*(struct t_process_context*)(system.process_info->current_process->val);
		if (_new_process_context.pid != _old_process_context.pid)
		{
				_processor_reg=_new_process_context.processor_reg;
		}                         
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) _new_process_context.page_dir)))
		DO_STACK_FRAME(_processor_reg.esp-8);	
		if (_action==2)                                                                              
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
		DO_STACK_FRAME(_processor_reg.esp-8);                                                                      
		RESTORE_PROCESSOR_REG                                                                       
		RET_FROM_INT_HANDLER                                                                        
	}
}
