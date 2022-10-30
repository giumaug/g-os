#include "asm.h"
#include "idt.h"
#include "virtual_memory/vm.h"  
#include "memory_manager/kmalloc.h" 
//#include "idt.h"
#include "drivers/lapic/lapic.h"
#include "drivers/kbc/8042.h" 
#include "lib/lib.h"
#include "debug.h"

extern ggo;
char lowercase_charset[0x80] = {
			[0x00] = '\0',			
			[0x01] = '\0',
			[0x02] = '1',
			[0x03] = '2',
			[0x04] = '3',
			[0x05] = '4',
			[0x06] = '5',		
			[0x07] = '6',
			[0x08] = '7',
			[0x09] = '8',
			[0x0a] = '9',
			[0x0b] = '0',
			[0x0c] = '-',
			[0x0d] = '=',
			[0x0e] = '\b',
			[0x0f] = '\0',
			[0x10] = 'q',
			[0x11] = 'w',
			[0x12] = 'e',
			[0x13] = 'r',
			[0x14] = 't',
			[0x15] = 'y',
			[0x16] = 'u',
			[0x17] = 'i',
			[0x18] = 'o',
			[0x19] = 'p',
			[0x1a] = '[',
			[0x1b] = ']',
			[0x1c] = '\r',
			[0x1d] = '\0',
			[0x1e] = 'a',
			[0x1f] = 's',
			[0x20] = 'd',
			[0x21] = 'f',
			[0x22] = 'g',
			[0x23] = 'h',
			[0x24] = 'j',
			[0x25] = 'k',
			[0x26] = 'l',
			[0x27] = ';',
			[0x28] = '"',
			[0x29] = '\0',
			[0x2a] = '\0', //left shift
			[0x2b] = '\\',
			[0x2c] = 'z',
			[0x2d] = 'x',
			[0x2e] = 'c',
			[0x2f] = 'v',
			[0x30] = 'b',
			[0x31] = 'n',
			[0x32] = 'm',
			[0x33] = ',',
			[0x34] = '.',
			[0x35] = '/',
			[0x36] = '\0', //right shift
			[0x37] = '\0',
			[0x38] = '\0',
			[0x39] = ' ',
			[0x3a] = '\0',
			[0x3b] = '\0',
			[0x3c] = '\0',
			[0x3d] = '\0',
			[0x3e] = '\0',
			[0x3f] = '\0',
			[0x40] = '\0',
			[0x41] = '\0',
			[0x42] = '\0',
			[0x43] = '\0',
			[0x44] = '\0',
			[0x45] = '\0',
			[0x46] = '\0',
			[0x47] = '\0',
			[0x48] = '\0',
			[0x49] = '\0',
			[0x4a] = '\0',
			[0x4b] = '\0',
			[0x4c] = '\0',
			[0x4d] = '\0',
			[0x4e] = '\0',
			[0x4f] = '\0',
			[0x50] = '\0',
			[0x51] = '\0',
			[0x52] = '\0',
			[0x53] = '\b',
			[0x54] = '\0',
			[0x55] = '\0',
			[0x56] = '\0',
			[0x57] = '\0',
			[0x58] = '\0',
			[0x59] = '\0',
			[0x5a] = '\0',
			[0x5b] = '\0',
			[0x5c] = '\0',
			[0x5d] = '\0',
			[0x5e] = '\0',
			[0x5f] = '\0',
			[0x60] = '\0',
			[0x61] = '\0',
			[0x62] = '\0',
			[0x63] = '\0',
			[0x64] = '\0',
			[0x65] = '\0',
			[0x66] = '\0',
			[0x67] = '\0',
			[0x68] = '\0',
			[0x69] = '\0',
			[0x6a] = '\0',
			[0x6b] = '\0',
			[0x6c] = '\0',
			[0x6d] = '\0',
			[0x6e] = '\0',
			[0x6f] = '\0',
			[0x70] = '\0',
			[0x71] = '\0',
			[0x72] = '\0',
			[0x73] = '\0',
			[0x74] = '\0',
			[0x75] = '\0',
			[0x76] = '\0',
			[0x77] = '\0',
			[0x78] = '\0',
			[0x79] = '\0',
			[0x7a] = '\0',
			[0x7b] = '\0',
			[0x7c] = '\0',
			[0x7d] = '\0',
			[0x7e] = '\0',
			[0x7f] = '\0'
};

char uppercase_charset[0x80] = {
			[0x00] = '\0',			
			[0x01] = '\0',
			[0x02] = '!',
			[0x03] = '@',
			[0x04] = '#',
			[0x05] = '$',
			[0x06] = '%',		
			[0x07] = '^',
			[0x08] = '&',
			[0x09] = '*',
			[0x0a] = '(',
			[0x0b] = ')',
			[0x0c] = '_',
			[0x0d] = '+',
			[0x0e] = '\b',
			[0x0f] = '\0',
			[0x10] = 'Q',
			[0x11] = 'W',
			[0x12] = 'E',
			[0x13] = 'R',
			[0x14] = 'T',
			[0x15] = 'Y',
			[0x16] = 'U',
			[0x17] = 'I',
			[0x18] = 'O',
			[0x19] = 'P',
			[0x1a] = '{',
			[0x1b] = '}',
			[0x1c] = '\r',
			[0x1d] = '\0',
			[0x1e] = 'A',
			[0x1f] = 'S',
			[0x20] = 'D',
			[0x21] = 'F',
			[0x22] = 'G',
			[0x23] = 'H',
			[0x24] = 'J',
			[0x25] = 'K',
			[0x26] = 'L',
			[0x27] = ':',
			[0x28] = '"',
			[0x29] = '\0',
			[0x2a] = '\0', //left shift
			[0x2b] = '|',
			[0x2c] = 'Z',
			[0x2d] = 'X',
			[0x2e] = 'C',
			[0x2f] = 'V',
			[0x30] = 'B',
			[0x31] = 'N',
			[0x32] = 'M',
			[0x33] = '<',
			[0x34] = '>',
			[0x35] = '?',
			[0x36] = '\0', //right shift
			[0x37] = '\0',
			[0x38] = '\0',
			[0x39] = ' ',
			[0x3a] = '\0',
			[0x3b] = '\0',
			[0x3c] = '\0',
			[0x3d] = '\0',
			[0x3e] = '\0',
			[0x3f] = '\0',
			[0x40] = '\0',
			[0x41] = '\0',
			[0x42] = '\0',
			[0x43] = '\0',
			[0x44] = '\0',
			[0x45] = '\0',
			[0x46] = '\0',
			[0x47] = '\0',
			[0x48] = '\0',
			[0x49] = '\0',
			[0x4a] = '\0',
			[0x4b] = '\0',
			[0x4c] = '\0',
			[0x4d] = '\0',
			[0x4e] = '\0',
			[0x4f] = '\0',
			[0x50] = '\0',
			[0x51] = '\0',
			[0x52] = '\0',
			[0x53] = '\b',
			[0x54] = '\0',
			[0x55] = '\0',
			[0x56] = '\0',
			[0x57] = '\0',
			[0x58] = '\0',
			[0x59] = '\0',
			[0x5a] = '\0',
			[0x5b] = '\0',
			[0x5c] = '\0',
			[0x5d] = '\0',
			[0x5e] = '\0',
			[0x5f] = '\0',
			[0x60] = '\0',
			[0x61] = '\0',
			[0x62] = '\0',
			[0x63] = '\0',
			[0x64] = '\0',
			[0x65] = '\0',
			[0x66] = '\0',
			[0x67] = '\0',
			[0x68] = '\0',
			[0x69] = '\0',
			[0x6a] = '\0',
			[0x6b] = '\0',
			[0x6c] = '\0',
			[0x6d] = '\0',
			[0x6e] = '\0',
			[0x6f] = '\0',
			[0x70] = '\0',
			[0x71] = '\0',
			[0x72] = '\0',
			[0x73] = '\0',
			[0x74] = '\0',
			[0x75] = '\0',
			[0x76] = '\0',
			[0x77] = '\0',
			[0x78] = '\0',
			[0x79] = '\0',
			[0x7a] = '\0',
			[0x7b] = '\0',
			[0x7c] = '\0',
			[0x7d] = '\0',
			[0x7e] = '\0',
			[0x7f] = '\0'
};

static t_queue* in_buf;
static t_llist* wait_queue; 

void static int_handler_kbc();

void init_kbc()
{	
	struct t_i_desc i_desc;
	
	in_buf=new_queue();
	i_desc.baseLow = ((int)&int_handler_kbc) & 0xFFFF;
	i_desc.selector = 0x8;
	i_desc.flags = 0x08e00; //0x0EF00;
	i_desc.baseHi = ((int) &int_handler_kbc) >> 0x10;
	set_idt_entry(0x21,&i_desc);
	out(0xad,0x64);
	in(0x60);
	out(0xae,0x64);
}

void free_kbc()
{
	kfree(in_buf);
}

void int_handler_kbc()
{
	u8 scan_code;
	u8* char_code = NULL;
	unsigned static int shift_state = 0;
	unsigned static int control_state = 0;
	struct t_processor_reg processor_reg;
	struct t_process_context* next_process_context;
	u32 fg_pgid;
	t_llist* pgid_list = NULL;
	t_llist_node* sentinel  = NULL;
	t_llist_node* next = NULL;
	
	SAVE_PROCESSOR_REG
	SWITCH_DS_TO_KERNEL_MODE
	DISABLE_PREEMPTION
	mask_entry(1);
	EOI_TO_LAPIC
	STI	
	
	scan_code = in(0x60);
	switch(scan_code) 
	{
		case 0x2a: 
			shift_state = 1; 
			break;
		case 0xaa: 
			shift_state = 0;
			break;
		case 0x1d:
			control_state = 1;
			break;
		case 0x9d:
			control_state = 0;
			break;
        default:
			// need exclude 56 and 29 because i use ctrl+alt in debug mode 
			if (!(scan_code & 0x80) && scan_code != 56) 
			{
				//All logic manages SIGINT should be outside interrupt handler!!!!
				if (control_state == 1 && scan_code == 0x2e)
				{
					SAVE_IF_STATUS
					CLI
					fg_pgid = system.active_console_desc->fg_pgid;
					if (fg_pgid !=1)
					{
						pgid_list = hashtable_get(system.process_info->pgid_hash,fg_pgid);
						if (pgid_list != NULL)
						{
							sentinel = ll_sentinel(pgid_list);
							next = ll_first(pgid_list);
							while(next != sentinel)
							{
								next_process_context = next->val;
								next_process_context->sig_num = SIGINT;
								if (next_process_context->pid == next_process_context->pgid)
								{
									next_process_context->parent->sig_num = SIGCHLD;
								}
								if (next_process_context->proc_status == SLEEPING)
								{
									_awake(next_process_context);
								}
								next = ll_next(next);
							}
						}
					}
					RESTORE_IF_STATUS
				}
				else
				{
					char_code = &(shift_state ? uppercase_charset : lowercase_charset)[scan_code];
					enqueue(in_buf,char_code);
					system.active_console_desc->is_empty = 0;
				}
			}
           	break;
	}
	unmask_entry(1);
	ENABLE_PREEMPTION
	// tmp put for test fake console. Put EXIT_INT_HANDLER    
	//RESTORE_PROCESSOR_REG                                                                            
	//RET_FROM_INT_HANDLER   
	EXIT_INT_HANDLER(0,processor_reg)                                                                              
/*                                                                                                                        
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
	_action2=0;                                                                                           
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
*/ 
}

char read_buf()
{
	char* char_code = NULL;
	char ret = NULL;
	u32 i;
	
	mask_entry(1);
	char_code = dequeue(in_buf);
	if (char_code != NULL) 
	{	
		ret = *char_code;
	}
	else system.active_console_desc->is_empty = 1;
	//CRAPPY SLEEP
	unmask_entry(1);
	for (i = 0; i < 1000000; i++);
	return ret;
}
