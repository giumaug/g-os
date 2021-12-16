#include "asm.h"
#include "idt.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "drivers/ata/ata.h"
#include "syscall_handler.h"
#include "debug.h"
#include "ext2/ext2.h"

#define K_STACK 0x1FFFFB

#define SELECT_FS(ext2) if (system.device_desc->num == 0)        \
			{                                        \
				ext2 = system.root_fs;           \
			}                                        \
			else                                     \
			{                                        \
				ext2 = system.scnd_fs;           \
			}                                        \

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
	u8 flush_network;
	t_ext2* ext2;

 	SAVE_PROCESSOR_REG
	//CLI
	//call can come from kernel mode (sleep)
	SWITCH_DS_TO_KERNEL_MODE
	syscall_num=processor_reg.eax;
	on_exit_action=0;
	current_process_context=system.process_info->current_process->val;
	t_console_desc *console_desc=current_process_context->console_desc;
	syscall_num=processor_reg.eax;
	params=processor_reg.ecx;
	flush_network = 0;

	//printk("syscall num = %d \n",syscall_num);
	//printk("fg_pgid = %d \n",console_desc->fg_pgid);

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
		//--SELECT_FS(ext2)
		if (system.device_desc->num == 0)        
		{                                        
			ext2 = system.root_fs;          
		}                                        
		else                                     
		{                                        
			ext2 = system.scnd_fs;           
		}   
	 	params[2] = _open(ext2,(char*) params[0],params[1]);
		on_exit_action=1;
		break; 

		case 19:
		SELECT_FS(ext2) 
		params[1]=_close(ext2,params[0]);
		on_exit_action=1; 
		break;

		case 20:
		SELECT_FS(ext2)
	 	params[3]=_read(ext2,params[0],params[1],params[2],1);
		on_exit_action=1; 
		break;

		case 36:
		SELECT_FS(ext2)
		params[3] = _seek(ext2,params[0],params[1],params[2]);
		on_exit_action=1; 
		break;
	
		case 22:
		SELECT_FS(ext2)
		params[1]=_rm(ext2,(char*)params[0]);
		on_exit_action=1; 
		break;

		case 23:
		SELECT_FS(ext2)
		params[1]=_mkdir(ext2,params[0]);
		on_exit_action=1; 
		break;

		case 26:
		SELECT_FS(ext2)
		params[1]=_chdir(system.root_fs,(char*) params[0]); 
		on_exit_action=1;
		break; 	
	
		case 27:
		SELECT_FS(ext2)
		params[2]=_stat(system.root_fs,(char*) params[0],params[1]);
		break;

		case 28:
 		params[1]=_open_socket(params[0]);
		break; 
	
		case 29:
 		params[5]=_bind(params[0],params[1],params[2],params[3],params[4]);
		break;	

		case 30:
		//printk("redc ");
 		params[5]=_recvfrom(params[0],params[1],params[2],params[3],params[4]);
		break;
	
		case 31:
 		params[5]=_sendto(params[0],params[1],params[2],params[3],params[4]);
		break;
	
		case 32:
 		params[0]=_close_socket(params[0]);
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

		case 37:
 		params[3] = _icmp_echo_request(params[0],params[1],params[2]);        
		break;

		case 38:
 		params[0] = _getpid();        
		break;

		case 39:
 		params[1] = _getpgid(params[0]);
		break;

		case 40:
 		params[2] = _setpgid(params[0],params[1]);      
		break;

		case 41:
 		params[0] = _tcgetpgrp();      
		break;

		case 42:
 		params[1] = _tcsetpgrp(params[0]);   
		break;

		case 101: 
		on_exit_action=1;
		break;
	
		case 106:
		params[1]=_listen(params[0]);
		break;

		case 107:
		_signal(params[0]);
		break;

		case 108:
		_select_dev(params[0]);
		break;

		case 109:
		SELECT_FS(ext2)
	 	params[4] = _read_write(ext2, params[0], params[1], params[2], params[3], 1);
		on_exit_action = 1;
		break;

		case 103:
		check_free_mem();
		break;

        case 104:
		panic();
		break;

		case 200:
		flush_inode_cache(system.root_fs);
		break;	

		default:
		panic();
	}
	if (syscall_num == 30 || syscall_num == 31 || syscall_num == 32 || syscall_num == 35)
	{
		system.flush_network = 1;
	}
	//EXIT_INT_HANDLER(on_exit_action,processor_reg)

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
	_action2=on_exit_action;                                                                                      
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
}
