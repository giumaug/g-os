#include "system.h"
#include "asm.h"
#include "idt.h"
#include "scheduler/process.h"
#include "memory_manager/kmalloc.h"
#include "memory_manager/buddy.h"
#include "virtual_memory/vm.h"
#include "syscall_handler.h"
#include "network/network.h"
#include "drivers/kbc/8042.h"
#include "drivers/ata/ata.h"

//to fix when file system working!!
unsigned int seed=105491;
extern unsigned int PAGE_DIR;
t_system system;

void kmain( void* mbd, unsigned int magic,int init_data_add)
{
	static struct t_process_info process_info;
	static t_scheduler_desc scheduler_desc;
	static t_buddy_desc buddy_desc;
	static unsigned int* init_data;
        init_data=init_data_add;
	static struct t_process_context* process_context = NULL;
	static struct t_i_desc i_desc;
	static t_console_desc console_desc;
	static t_ext2 ext2;
	static t_device_desc device_desc;
	static u32 kernel_stack;

	system.fork = 0;
	system.out = 0;
	system.tcp_1 = 0;
	system.piggy_timeout = 0;
	system.rtrsn_timeout = 0;
	system.time = 0;
	system.flush_network = 0;
	system.tot_int = 0;
	system.packet_sent = 0;
	system.packet_received = 0;
	system.counter = 0;
	system.max_processed_packet = 0;
	system.sleep_count = 0;

	#ifdef PROFILE
	system.time_counter_1 = 0;
	system.time_counter_2 = 0;
	system.time_counter_3 = 0;
	system.time_counter_4 = 0;
	system.time_counter_5 = 0;
	system.time_counter_6 = 0;
	system.time_counter_7 = 0;
	system.time_counter_8 = 0;
	system.time_counter_9 = 0;
	system.time_counter_10 = 0;
	system.time_counter_11 = 0;
	system.time_counter_12 = 0;
	system.exec_time_1 = 0;
	system.exec_time_2 = 0;
	system.exec_time_3 = 0;
	system.exec_time_4 = 0;
	system.exec_time_5 = 0;
	system.exec_time_6 = 0;
	system.exec_time_7 = 0;
	system.exec_time_8 = 0;
	system.exec_time_9 = 0;
	system.exec_time_10 = 0;
	system.exec_time_11 = 0;
	system.exec_time_12 = 0;
	system.tot_mem_kmemcpy = 0;
	#endif
	system.tot_sent = 0;

	init_data = init_data_add;
	if ( magic != 0x2BADB002 )
   	{
      		/* Something went not according to specs. Print an error */
   	}
 	CLI
	system.force_scheduling = 0;
	system.process_info = &process_info;
	system.buddy_desc = &buddy_desc;
	system.scheduler_desc = &scheduler_desc;
	system.int_path_count = 0;
	system.scheduler_desc->scheduler_queue[0] = 0;
	system.process_info->current_process = NULL;
	init_kmalloc();
   	init_idt();
   	init_pic();
   	init_pit();
	init_kbc();
	init_console(&console_desc,4000,0);
	buddy_init(system.buddy_desc);
	init_scheduler();
	init_ata(&device_desc);
	init_ext2(&ext2,&device_desc);
	system.root_fs = &ext2;
	system.device_desc = &device_desc;
	
	system.master_page_dir = init_virtual_memory();
	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int)system.master_page_dir)))
	system.active_console_desc = &console_desc;
	i_desc.baseLow = ((int)&syscall_handler) & 0xFFFF;
	i_desc.selector = 0x8;
	i_desc.flags = 0x0EF00; 
	i_desc.baseHi = ((int)&syscall_handler)>>0x10;
	set_idt_entry(0x80,&i_desc);

	system.process_info->sleep_wait_queue = new_dllist();	
	system.process_info->process_context_list = new_dllist();
	system.process_info->next_pid = 1;
	
	process_context = kmalloc(sizeof(struct t_process_context));
	process_context->root_dir_inode_number = ROOT_INODE;
	process_context->current_dir_inode_number = ROOT_INODE;
	process_context->proc_status = RUNNING;
	process_context->assigned_sleep_time = 0;	
	process_context->sleep_time = 0;
	process_context->static_priority = 0;
	process_context->curr_sched_queue_index = 9;
	process_context->pid = 0;
        process_context->tick = TICK;
	process_context->processor_reg.esp = NULL;
	process_context->console_desc = &console_desc;
	system.process_info->current_process = ll_prepend(system.scheduler_desc->scheduler_queue[9],process_context);
	system.process_info->tss.ss = *init_data;
	system.process_info->tss.esp = *(init_data+1);
	system.process_info->pause_queue = new_dllist();
	process_context->phy_kernel_stack = FROM_VIRT_TO_PHY(buddy_alloc_page(system.buddy_desc,KERNEL_STACK_SIZE));
	process_context->process_type = KERNEL_THREAD;
	process_context->file_desc = dc_hashtable_init(PROCESS_INIT_FILE,&inode_free);
	process_context->socket_desc = hashtable_init(PROCESS_INIT_SOCKET);
	process_context->next_sd = 0;
	 
	process_context->page_dir = buddy_alloc_page(system.buddy_desc,0x1000);                      
	init_vm_process(process_context);
	*(system.process_info->tss.ss) = 0x18;
	*(system.process_info->tss.esp) = KERNEL_STACK;
	system.network_desc = network_init();
	system.timer_list = new_dllist();                       		
	kernel_stack = KERNEL_STACK - 100;
	asm volatile ("movl %0,%%ebp;"::"r"(kernel_stack));
	asm volatile ("movl %0,%%esp;"::"r"(kernel_stack));
	STI
	process_0();
//	long ebp;
//	long esp;
//	asm("mov %%ebp,%0;":"=r"(ebp));
//	asm("mov %%esp,%0;":"=r"(esp));
//	if (ebp == esp == 0) while(1);			       	
}

void panic()
{
	printk("\n");
	printk("Kernel panic!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	printk("\n");
	//while(1);
}
