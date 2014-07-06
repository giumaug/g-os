#include "debug.h"
#include "general.h"
#include "system.h"
#include "scheduler/scheduler.h"
#include "memory_manager/buddy.h"
#include "virtual_memory/vm.h"
#include "asm.h"
#include "lib/lib.h"

extern t_system system;
extern struct t_llist* kbc_wait_queue;
extern unsigned int *master_page_dir;

void do_context_switch(struct t_process_context *current_process_context,
		       struct t_processor_reg *processor_reg,
		       struct t_process_context *new_process_context)
{
	*(system.process_info.tss.esp)=0x1FFFFF;
	//save current process state 
	current_process_context->processor_reg.eax=processor_reg->eax;
	current_process_context->processor_reg.ebx=processor_reg->ebx;
	current_process_context->processor_reg.ecx=processor_reg->ecx;
	current_process_context->processor_reg.edx=processor_reg->edx;
	current_process_context->processor_reg.esi=processor_reg->esi;
	current_process_context->processor_reg.edi=processor_reg->edi;
	current_process_context->processor_reg.esp=processor_reg->esp;

	//restore new process state
	processor_reg->eax=new_process_context->processor_reg.eax;
	processor_reg->ebx=new_process_context->processor_reg.ebx;
	processor_reg->ecx=new_process_context->processor_reg.ecx;
	processor_reg->edx=new_process_context->processor_reg.edx;
	processor_reg->esi=new_process_context->processor_reg.esi;
	processor_reg->edi=new_process_context->processor_reg.edi;
	processor_reg->esp=new_process_context->processor_reg.esp;
}

void init_scheduler()
{
	int i;
	for (i=0;i<10;i++)
	{
		system.scheduler_desc.scheduler_queue[i]=new_dllist();
	}
}

//SCHEDULE SHOULD BE OUTSIDE SLEEP PAUSE ECC... TO AVOID PROPAGATION OF PROCESSOR_REG.THIS
//REQUIRE SOME LOT OF REFACTORING ON CODE 
void schedule(struct t_process_context *current_process_context,struct t_processor_reg *processor_reg)
{
//	struct t_process_context* current_process_context;
	struct t_process_context* next_process_context;
	t_llist_node* node;
	t_llist_node* next;
	t_llist_node* sentinel_node;
	unsigned int stop=0;
	unsigned int queue_index;
	unsigned int priority;
	unsigned int index;	

	index=0;
	node=system.process_info.current_process;	
	current_process_context=node->val;
	
	while(!stop && index<10)
	{
		sentinel_node=ll_sentinel(system.scheduler_desc.scheduler_queue[index]);
		next=ll_first(system.scheduler_desc.scheduler_queue[index]);
		while(next!=sentinel_node && !stop)
		{
			next_process_context=next->val;
			if (current_process_context->pid!=next_process_context->pid)
			{
				do_context_switch(current_process_context,processor_reg,next_process_context);	
				system.process_info.current_process=next;
				if (current_process_context->proc_status==RUNNING)
				{
					adjust_sched_queue(current_process_context);
					ll_delete_node(node);
					queue_index=current_process_context->curr_sched_queue_index;
					ll_append(system.scheduler_desc.scheduler_queue[queue_index],current_process_context);
				}
				else if (current_process_context->proc_status==SLEEPING)
				{
					ll_delete_node(node);	
				}
				else if (current_process_context->proc_status==EXITING)
				{
					kfree(current_process_context);
					ll_delete_node(node);
				}
				stop=1;
			}
			else 
			{
				next=ll_next(next);
			}	
		}
		index++; 
	}
}

void adjust_sched_queue(struct t_process_context *current_process_context)
{
	int priority;
	unsigned int queue_index;
		
	//static prioriry range from -10 to 10 default value is 0;
	priority=current_process_context->sleep_time+(current_process_context->static_priority*10);

	if (priority>=0 && priority<100)
	{
		queue_index=9;
	}
	else 
	{	
		if (priority>=100 && priority<200)
		{
			queue_index=8;
		}
		else 
		{
			if (priority>=200 && priority<300)
			{
				queue_index=7;
			}
			else 
			{
				if (priority>=300 && priority<400)
				{
					queue_index=6;
				}
				else 
				{
					if (priority>=400 && priority<500)
					{
						queue_index=5;
					}
					else
					{
						if (priority>=500 && priority<600)
						{
							queue_index=4;
						}
						else
						{
							if (priority>=600 && priority<700)
							{
								queue_index=3;
							}
							else
							{
								if (priority>=700 && priority<800)
								{
									queue_index=2;
								}
								else
								{
									if (priority>=800 && priority<900)
									{
										queue_index=1;
									}
									else
									{
										if (priority>=900 && priority<=1000)
										{
											queue_index=0;
										}	
									}	
								}	
							}	
						}
					}	
				}	
			}
		}		
	}
	current_process_context->curr_sched_queue_index=queue_index;
	return;
}

void _sleep()
{
	struct t_process_context* current_process;
	SAVE_IF_STATUS
	CLI        
	current_process=system.process_info.current_process->val;
	current_process->sleep_time=system.time;
	t_llist_node* current_node=system.process_info.current_process;
	current_process->proc_status=SLEEPING;
	RESTORE_IF_STATUS
	SUSPEND
}

void _awake(struct t_process_context *new_process)
{
	t_llist_node* new_node;

	SAVE_IF_STATUS
	CLI
	new_process->sleep_time=(system.time-new_process->sleep_time>=1000) ? 1000 : (system.time-new_process->sleep_time);
	new_process->proc_status=RUNNING;
	adjust_sched_queue(new_process);
	ll_prepend(system.scheduler_desc.scheduler_queue[new_process->curr_sched_queue_index],new_process);
	RESTORE_IF_STATUS
}

void _pause()
{
	struct t_process_context* current_process;
	t_llist* pause_queue;

	SAVE_IF_STATUS
	CLI
	pause_queue=system.process_info.pause_queue;
	current_process=system.process_info.current_process->val;
	ll_prepend(pause_queue,current_process);	
	_sleep();
	RESTORE_IF_STATUS
}

void _exit(int status)
{
	unsigned int exit_action=2;
	t_llist_node* next;
	t_llist_node* sentinel;
	unsigned int awake_process=0;
	struct t_process_context* current_process;
	struct t_process_context* next_process;
	unsigned int* ret;
	
	SAVE_IF_STATUS
	CLI
	t_llist_node* current_node=system.process_info.current_process;
	//process 0 never die
	current_process=system.process_info.current_process->val;
	if (current_process->pid==0)
	{
		while(1)
		{
			asm("sti;hlt");
		}
	}
	current_process->proc_status=EXITING;
	sentinel=ll_sentinel(system.process_info.pause_queue);
	next=ll_first(system.process_info.pause_queue);
	next_process=next->val;
	while(next!=sentinel && !awake_process)
	{	
		if (next_process->pid==current_process->parent->pid)
		{
			awake_process=1;
		}
		if (awake_process)
		{
			_awake(next->val);
			ll_delete_node(next);
		}
		next=ll_next(next);
		next_process=next->val;
	}
	RESTORE_IF_STATUS
}

int _fork(struct t_processor_reg processor_reg,unsigned int flags) 
{
	unsigned int parent_add_space;
	unsigned int child_add_space;
	unsigned int stack_bottom;
	unsigned int i=0;
	unsigned int mem_size;
	unsigned int esp;
	unsigned int eip;
	char *proc_mem;

 	struct t_process_context* child_process_context;
	struct t_process_context* parent_process_context;

	child_process_context=kmalloc(sizeof(struct t_process_context));
	SAVE_IF_STATUS
	CLI
	parent_process_context=system.process_info.current_process->val;
	kmemcpy(child_process_context,parent_process_context,sizeof(struct t_process_context));
	child_process_context->pid=system.process_info.next_pid++;
	child_process_context->parent=parent_process_context;
	child_process_context->processor_reg=processor_reg;
	child_process_context->phy_add_space=NULL;
	if (flags==INIT_VM_USERSPACE)
	{
		mem_size=parent_process_context->phy_space_size;
		proc_mem=buddy_alloc_page(&system.buddy_desc,mem_size);    
		child_process_context->phy_add_space=FROM_VIRT_TO_PHY(proc_mem); 
		kmemcpy(proc_mem,FROM_PHY_TO_VIRT(parent_process_context->phy_add_space),mem_size);
	}
	ll_prepend(system.scheduler_desc.scheduler_queue[parent_process_context->curr_sched_queue_index],child_process_context);
	child_process_context->page_dir=init_vm_process(system.master_page_dir,child_process_context->phy_add_space,child_process_context,flags);
	RESTORE_IF_STATUS
	return child_process_context->pid;
}

void _exec(char* path) 
{
	struct t_process_context *current_process_context;
	char* process_storage;
//	char* process_space;
	unsigned int i=0;
	static unsigned int old_proc_phy_addr;
	static void* old_page_dir;
	
//	SAVE_IF_STATUS
	CLI
	current_process_context=system.process_info.current_process->val;
	current_process_context->proc_status=RUNNING;
	current_process_context->sleep_time=0;
	current_process_context->assigned_sleep_time=0;
	current_process_context->static_priority=0;
//	current_process_context->phy_space_size=size;
//	process_space=buddy_alloc_page(&system.buddy_desc,size);
//	process_storage=FROM_PHY_TO_VIRT(start_addr);
	old_page_dir=current_process_context->page_dir;
	old_proc_phy_addr=current_process_context->phy_add_space;

	load_elf_executable(path,current_process_context); 

//	current_process_context->phy_add_space=FROM_VIRT_TO_PHY(process_space);
//	for (i=0;i<size;i++)
//	{
//		*process_space++=*process_storage++;
//	}
	current_process_context->page_dir=init_vm_process(system.master_page_dir,current_process_context->phy_add_space,current_process_context,INIT_VM_USERSPACE);
	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir)))
	free_vm_process(old_page_dir,INIT_VM_USERSPACE);
	buddy_free_page(&system.buddy_desc,FROM_PHY_TO_VIRT(old_proc_phy_addr));                        	
	SWITCH_TO_USER_MODE
}

void _sleep_time(unsigned int time)
{
	struct t_process_context* current_process;
	t_llist* sleep_wait_queue;

	SAVE_IF_STATUS	
	CLI 
	sleep_wait_queue=system.process_info.sleep_wait_queue;
	current_process=system.process_info.current_process->val;
	current_process->assigned_sleep_time=time;
	ll_prepend(sleep_wait_queue,current_process);	
	_sleep();
	RESTORE_IF_STATUS
}
