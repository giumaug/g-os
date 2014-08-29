#include "general.h"
#include "asm.h"
#include "idt.h"
#include "scheduler/process.h"
#include "memory_manager/kmalloc.h"
#include "memory_manager/buddy.h"
#include "virtual_memory/vm.h"
#include "syscall_handler.h"
#include "drivers/kbc/8042.h"
#include "drivers/ata/ata.h"

//to fix when file system working!!
unsigned int seed=105491;
extern unsigned int PAGE_DIR;
t_system system;
char tmp_kernel_stack[4096];

void kmain( void* mbd, unsigned int magic,int init_data_add)
{
	static struct t_process_info process_info;
	static t_buddy_desc buddy_desc;
	static unsigned int* init_data;
        init_data=init_data_add;
	static struct t_process_context* process_context;
	static struct t_i_desc i_desc;
	static t_console_desc console_desc;
	static t_ext2 ext2;
	static t_device_desc device_desc;

	system.time=0;
	init_data=init_data_add;
	asm ("movl %0,%%esp;"::"r"(tmp_kernel_stack+0x1000));
	asm ("movl %0,%%ebp;"::"r"(tmp_kernel_stack+0x1000));

	
	if ( magic != 0x2BADB002 )
   	{
      		/* Something went not according to specs. Print an error */
   	}
 	CLI
	system.process_info=&process_info;
	system.buddy_desc=&buddy_desc;
	system.int_path_count=0;
	system.scheduler_desc.scheduler_queue[0]=0;
	system.process_info->current_process=NULL;
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
	system.root_fs=&ext2;
	system.device_desc=&device_desc;
	
	system.master_page_dir=init_virtual_memory();
	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int)system.master_page_dir)))
	system.active_console_desc=&console_desc;
	i_desc.baseLow=((int)&syscall_handler) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x0EF00; 
	i_desc.baseHi=((int)&syscall_handler)>>0x10;
	set_idt_entry(0x80,&i_desc);

	system.process_info->sleep_wait_queue=new_dllist();	
	system.process_info->process_context_list=new_dllist();
	system.process_info->next_pid=1;
	
	process_context=kmalloc(sizeof(struct t_process_context));
	process_context->proc_status=RUNNING;
	process_context->assigned_sleep_time=0;	
	process_context->sleep_time=0;
	process_context->static_priority=0;
	process_context->curr_sched_queue_index=9;
	process_context->pid=0;
        process_context->tick=TICK;
        process_context->processor_reg.esp=0x1EFFFF;//64K user mode stack 
	process_context->console_desc=&console_desc;
	system.process_info->current_process=ll_prepend(system.scheduler_desc.scheduler_queue[9],process_context);
	system.process_info->tss.ss= *init_data;
	system.process_info->tss.esp= *(init_data+1);
	system.process_info->pause_queue=new_dllist();
	process_context->phy_add_space=NULL;
	process_context->phy_k_thread_stack=FROM_VIRT_TO_PHY(buddy_alloc_page(system.buddy_desc,0x10000));
	//*process_context->current_dir="/";
	
//	process_space=buddy_alloc_page(&system.buddy_desc,0x100000);
//	process_storage=FROM_PHY_TO_VIRT(0x500000);
//	proc_phy_addr=FROM_VIRT_TO_PHY(process_space);
//	process_context.phy_add_space=proc_phy_addr;
//	process_context.phy_space_size=0x100000;
//	for (i=0;i<0x100000;i++)
//	{
//		*process_space++=*process_storage++;
//	}                         
	process_context->page_dir=init_vm_process(system.master_page_dir,process_context->phy_add_space,process_context,NO_INIT_VM_USERSPACE);
	*(system.process_info->tss.ss)=0x18;
	*(system.process_info->tss.esp)=0x1FFFFF;//64K kernel mode stack
	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) process_context->page_dir)))                           	
	SWITCH_TO_USER_MODE(data)
	asm("movl $0x1FFFFF,%ebp");
	asm("movl $0x1FFFFF,%esp");
	STI
	process_0();				       	
}
