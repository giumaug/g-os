#include "scheduler/scheduler.h"
#include "memory_manager/buddy.h"
#include "virtual_memory/vm.h"
#include "asm.h"
#include "lib/lib.h"

extern struct t_llist* kbc_wait_queue;
extern unsigned int *master_page_dir;

void do_context_switch(struct t_process_context *current_process_context,
		       struct t_processor_reg *processor_reg,
		       struct t_process_context *new_process_context)
{
	*(system.process_info->tss.esp)=KERNEL_STACK;
	//save current process state 
	current_process_context->processor_reg.eax=processor_reg->eax;
	current_process_context->processor_reg.ebx=processor_reg->ebx;
	current_process_context->processor_reg.ecx=processor_reg->ecx;
	current_process_context->processor_reg.edx=processor_reg->edx;
	current_process_context->processor_reg.esi=processor_reg->esi;
	current_process_context->processor_reg.edi=processor_reg->edi;
	current_process_context->processor_reg.esp=processor_reg->esp;
	current_process_context->processor_reg.ds=processor_reg->ds;
	current_process_context->processor_reg.es=processor_reg->es;

	//restore new process state
	processor_reg->eax=new_process_context->processor_reg.eax;
	processor_reg->ebx=new_process_context->processor_reg.ebx;
	processor_reg->ecx=new_process_context->processor_reg.ecx;
	processor_reg->edx=new_process_context->processor_reg.edx;
	processor_reg->esi=new_process_context->processor_reg.esi;
	processor_reg->edi=new_process_context->processor_reg.edi;
	processor_reg->esp=new_process_context->processor_reg.esp;
	processor_reg->ds=current_process_context->processor_reg.ds;
	processor_reg->es=current_process_context->processor_reg.es;

}

void init_scheduler()
{
	int i;
	for (i=0;i<10;i++)
	{
		system.scheduler_desc->scheduler_queue[i]=new_dllist();
	}
}

//SCHEDULE SHOULD BE OUTSIDE SLEEP PAUSE ECC... TO AVOID PROPAGATION OF PROCESSOR_REG.THIS
//REQUIRE SOME REFACTORING ON CODE 
void schedule(struct t_process_context *current_process_context,struct t_processor_reg *processor_reg)
{
	struct t_process_context* next_process_context;
	t_llist_node* node;
	t_llist_node* next;
	t_llist_node* sentinel_node;
	unsigned int stop=0;
	unsigned int queue_index;
	unsigned int priority;
	unsigned int index;	

	index=0;
	node=system.process_info->current_process;	
	current_process_context=node->val;

	while(!stop && index<10)
	{
		sentinel_node=ll_sentinel(system.scheduler_desc->scheduler_queue[index]);
		next=ll_first(system.scheduler_desc->scheduler_queue[index]);
		while(next!=sentinel_node && !stop)
		{
			next_process_context=next->val;
			if (current_process_context->pid!=next_process_context->pid && next_process_context->pid!=0)
			{
				do_context_switch(current_process_context,processor_reg,next_process_context);	
				system.process_info->current_process=next;
				if (current_process_context->proc_status==RUNNING)
				{
					adjust_sched_queue(current_process_context);
					ll_delete_node(node);
					queue_index=current_process_context->curr_sched_queue_index;
					ll_append(system.scheduler_desc->scheduler_queue[queue_index],current_process_context);
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
	if (stop == 0)
	{
		if (current_process_context->proc_status == RUNNING)
		{
			adjust_sched_queue(current_process_context);
		}
		else 
		{
			do_context_switch(current_process_context,processor_reg,system.process_info->process_0);	
			system.process_info->current_process = system.process_info->process_0;
			if (current_process_context->proc_status==SLEEPING)
			{
				ll_delete_node(node);
			}
			else if (current_process_context->proc_status==EXITING)
			{
				kfree(current_process_context);
				ll_delete_node(node);
			}
		}
	}
}

void adjust_sched_queue(struct t_process_context *current_process_context)
{
	int priority;
	unsigned int queue_index;
		
	//static prioriry range from -10 to 10 default value is 0;
	priority=current_process_context->sleep_time+(current_process_context->static_priority*10);

	switch(priority)
	{
		case 0 ... 99:
		{
			queue_index=9;
			break;
		}		
		case 100 ... 199:
		{
			queue_index=8;
			break;
		}
		case 200 ... 299:
		{
			queue_index=7;
			break;
		}
		case 300 ... 399:
		{
			queue_index=6;
			break;
		}
		case 400 ... 499:
		{
			queue_index=5;
			break;
		}
		case 500 ... 599:
		{
			queue_index=4;
			break;
		}
		case 600 ... 699:
		{
			queue_index=3;
			break;
		}
		case 700 ... 799:
		{
			queue_index=2;
			break;
		}
		case 800 ... 899:
		{
			queue_index=1;
			break;
		}
		case 900 ... 1000:
		{
			queue_index=0;
			break;
		}		
	}
	current_process_context->curr_sched_queue_index=queue_index;
	return;
}

void _sleep()
{
	_sleep_and_unlock(NULL);
}

void _sleep_and_unlock(t_spinlock_desc* lock)
{
	t_spinlock_desc aa;
	struct t_process_context* current_process;
	SAVE_IF_STATUS
	CLI
	//printk("1");        
	current_process=system.process_info->current_process->val;
	current_process->sleep_time=system.time;
	t_llist_node* current_node=system.process_info->current_process;
	int xxx = current_process->proc_status;
	current_process->proc_status=SLEEPING;
	if (lock!=NULL)
	{
		SPINLOCK_UNLOCK(*lock);
	}
	//Preemption has to be re-enabled OKKIO!!!!!!!!!
	//system.int_path_count = 0;
	//printk("2");
//	INT WILL BE DISABLED UNTIL SYSCALL HANDLER EXIT
	SUSPEND
	RESTORE_IF_STATUS
}

void _awake(struct t_process_context *new_process)
{
	t_llist_node* new_node;
	struct t_process_context* process_context;

	SAVE_IF_STATUS
	CLI
	CURRENT_PROCESS_CONTEXT(process_context);
	new_process->sleep_time=(system.time-new_process->sleep_time>=1000) ? 1000 : (system.time-new_process->sleep_time);
	new_process->proc_status=RUNNING;
	adjust_sched_queue(new_process);
	//COULD ARRIVE AN ATA INTERRUPT DURING NETWORK FLUSH
	if (process_context->pid != new_process->pid)
	{
		ll_prepend(system.scheduler_desc->scheduler_queue[new_process->curr_sched_queue_index],new_process);
	}
	else
	{
		//printk("s");
	}
	system.force_scheduling = 1;
	RESTORE_IF_STATUS
}

void _pause()
{
	struct t_process_context* current_process;
	t_llist* pause_queue;

	SAVE_IF_STATUS
	CLI
	pause_queue=system.process_info->pause_queue;
	current_process=system.process_info->current_process->val;
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
	system.out++;
	//process 0 never die
	CURRENT_PROCESS_CONTEXT(current_process);
	if (current_process->pid==0)
	{
		while(1) 
		{
			asm("sti;hlt");
		}
	}
	current_process->proc_status=EXITING;
	sentinel=ll_sentinel(system.process_info->pause_queue);
	next=ll_first(system.process_info->pause_queue);
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
	if (current_process->elf_desc!=NULL)
	{
		elf_loader_free(current_process->elf_desc);
		kfree(current_process->elf_desc);
	}
	if (current_process->process_type==USERSPACE_PROCESS)
	{
		delete_mem_reg(current_process->process_mem_reg);
		delete_mem_reg(current_process->heap_mem_reg);
		delete_mem_reg(current_process->ustack_mem_reg);
	}
	
	hashtable_free(current_process->file_desc);
	hashtable_free(current_process->socket_desc);

	#ifdef PROFILE
	if (current_process->pid == 2)	 //2
	{
		int i = 0;
		long long time_1;
		rdtscl(&time_1);
		system.time_counter_4++;
		system.timeboard_4[system.time_counter_4] = time_1;

		for (i = 2;i < system.time_counter_1;i++)
		{
			system.exec_time_1 = system.exec_time_1 + system.timeboard_1[i];	
		}
		for (i = 2;i < system.time_counter_2;i++)
		{
			system.exec_time_2 = system.exec_time_2 + system.timeboard_2[i];	
		}
		for (i = 2;i < system.time_counter_3;i++)
		{
			system.exec_time_3 = system.exec_time_3 + system.timeboard_3[i];	
		}
		for (i = 2;i < system.time_counter_5;i++)
		{
			system.exec_time_5 = system.exec_time_5 + system.timeboard_5[i];	
		}
		for (i = 2;i < system.time_counter_6;i++)
		{
			system.exec_time_6 = system.exec_time_6 + system.timeboard_6[i];	
		}
		for (i = 2;i < system.time_counter_7;i++)
		{
			system.exec_time_7 = system.exec_time_7 + system.timeboard_7[i];	
		}
		for (i = 2;i < system.time_counter_8;i++)
		{
			system.exec_time_8 = system.exec_time_8 + system.timeboard_8[i];	
		}
		for (i = 2;i < system.time_counter_9;i++)
		{
			system.exec_time_9 = system.exec_time_9 + system.timeboard_9[i];	
		}
		for (i = 2;i < system.time_counter_10;i++)
		{
			system.exec_time_10 = system.exec_time_10 + system.timeboard_10[i];	
		}
		for (i = 2;i < system.time_counter_11;i++)
		{
			system.exec_time_11 = system.exec_time_11 + system.timeboard_11[i];	
		}
		for (i = 2;i < system.time_counter_12;i++)
		{
			system.exec_time_12 = system.exec_time_12 + system.timeboard_12[i];	
		}
		system.tot_mem_kmemcpy = system.exec_time_11 + system.exec_time_7 + system.exec_time_8 + system.exec_time_9;
		printk("exit \n");
	}
	#endif
	RESTORE_IF_STATUS
}

int _fork(struct t_processor_reg processor_reg) 
{
	system.fork++;
	int i = 0;
 	struct t_process_context* child_process_context = NULL;
	struct t_process_context* parent_process_context = NULL;
	t_hashtable* child_file_desc = NULL;
	t_hashtable* child_socket_desc = NULL;
	char* kernel_stack_addr = NULL;
	t_elf_desc* child_elf_desc = NULL;

	if (system.process_info->next_pid == 2)	
	{
		#ifdef PROFILE
		long long time_1;
		rdtscl(&time_1);
		system.time_counter_4++;
		system.timeboard_4[system.time_counter_4] = time_1;
		printk("fork \n");
		#endif
	}
	
	child_process_context = kmalloc(sizeof(struct t_process_context));
	SAVE_IF_STATUS
	CLI
	CURRENT_PROCESS_CONTEXT(parent_process_context);
	if (parent_process_context->pid == 0)
	{
		system.process_info->process_0 = system.process_info->current_process;
	}
	kmemcpy(child_process_context,parent_process_context,sizeof(struct t_process_context));
	child_process_context->processor_reg = processor_reg;
	kernel_stack_addr = buddy_alloc_page(system.buddy_desc,KERNEL_STACK_SIZE);
	child_process_context->phy_kernel_stack = FROM_VIRT_TO_PHY(kernel_stack_addr);
	kmemcpy(kernel_stack_addr,FROM_PHY_TO_VIRT(parent_process_context->phy_kernel_stack),KERNEL_STACK_SIZE);
	child_process_context->pid = system.process_info->next_pid++;

	child_process_context->parent = parent_process_context;
//	TEMPORARY TRICK.CORRECT SOLUTION IS TO ADD A POINTER TO CLONER FUNTION IN HASMAP CLONE ALONSIDE DESTRUCTOR POINTER
//	ALSO INODE SHOULD NOT BE CLONED!!!!!!!	
	child_process_context->file_desc = clone_file_desc(parent_process_context->file_desc);
	child_process_context->socket_desc = clone_socket_desc(parent_process_context->socket_desc);

	if (parent_process_context->process_type == USERSPACE_PROCESS)
	{	
		child_elf_desc = kmalloc(sizeof(t_elf_desc));
		kmemcpy(child_elf_desc,parent_process_context->elf_desc,sizeof(t_elf_desc));

		child_process_context->elf_desc = child_elf_desc;
		child_process_context->process_mem_reg = create_mem_reg(parent_process_context->process_mem_reg->start_addr,
								      parent_process_context->process_mem_reg->end_addr);

		child_process_context->heap_mem_reg = create_mem_reg(parent_process_context->heap_mem_reg->start_addr,
								   parent_process_context->heap_mem_reg->end_addr);

		child_process_context->ustack_mem_reg = create_mem_reg(parent_process_context->ustack_mem_reg->start_addr,
								     parent_process_context->ustack_mem_reg->end_addr);	
	}
	ll_prepend(system.scheduler_desc->scheduler_queue[parent_process_context->curr_sched_queue_index],child_process_context);
	child_process_context->page_dir = clone_vm_process(parent_process_context->page_dir,
							 parent_process_context->process_type,
							 FROM_VIRT_TO_PHY(kernel_stack_addr));

	child_process_context->start_time = system.time;
	RESTORE_IF_STATUS
	return child_process_context->pid;
}

extern unsigned int collect_mem;

u32 _exec(char* path,char* argv[]) 
{
	struct t_process_context* current_process_context;
	char** bk_area;
	u32 data_size;
	u32* stack_pointer;
	u32* stack_data_pointers;
	char* page_addr;
	char* stack_data;
	u32 argc = 0;
	u32 i = 0;
	u32 j = 0;
	u32 k = 0;
	u32 z = 0;
	u32 frame_size = 0;
	u32 process_size;
	t_elf_desc* elf_desc;

	CURRENT_PROCESS_CONTEXT(current_process_context);
	if (current_process_context->elf_desc == NULL)
	{
		elf_desc=kmalloc(sizeof(t_elf_desc));
		current_process_context->elf_desc = elf_desc;
	}

	hashtable_free(current_process_context->file_desc);
	hashtable_free(current_process_context->socket_desc);
	current_process_context->file_desc = dc_hashtable_init(PROCESS_INIT_FILE,&inode_free);
	current_process_context->socket_desc = dc_hashtable_init(PROCESS_INIT_SOCKET,&socket_free);
	if (elf_loader_init(current_process_context->elf_desc,path) == -1)
	{
		return -1;
	}
	process_size=current_process_context->elf_desc->file_size;
	current_process_context->proc_status=RUNNING;
	current_process_context->sleep_time=0;
	current_process_context->assigned_sleep_time= 0;
	current_process_context->static_priority=0;

	i = 0;
	data_size = 0;
	while(argv[i++] != NULL)
	{
		 argc++;
	}

	bk_area = kmalloc(sizeof(char*)*argc);				
	for(k = 0;k < argc;k++)
	{
		i=0;
		while (argv[k][i] != NULL)
		{
			i++;		
		}
		bk_area[k] = kmalloc(i+1);
		data_size += (i+1);
	}

	for(k = 0; k< argc;k++)
	{
		i = 0;
		while (argv[k][i] != NULL)
		{		
			bk_area[k][i] = argv[k][i];
			i++;
		}
		bk_area[k][i++] = '\0';
	}		
	if (current_process_context->process_type == KERNEL_THREAD)
	{
		current_process_context->process_mem_reg = create_mem_reg(PROC_VIRT_MEM_START_ADDR,PROC_VIRT_MEM_START_ADDR+process_size);
		current_process_context->heap_mem_reg = create_mem_reg(HEAP_VIRT_MEM_START_ADDR,HEAP_VIRT_MEM_START_ADDR+HEAP_INIT_SIZE);
		current_process_context->ustack_mem_reg = create_mem_reg(USER_STACK-USER_STACK_INIT_SIZE,USER_STACK);
		page_addr = buddy_alloc_page(system.buddy_desc,PAGE_SIZE);
		map_vm_mem(current_process_context->page_dir,(USER_STACK-PAGE_SIZE),FROM_VIRT_TO_PHY(page_addr),PAGE_SIZE,7);
		system.buddy_desc->count[BLOCK_INDEX_FROM_PHY(FROM_VIRT_TO_PHY((unsigned int)page_addr))]++;
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir))) 
	}
	else
	{
		free_vm_process_user_space(current_process_context);
		current_process_context->process_mem_reg->end_addr = PROC_VIRT_MEM_START_ADDR+process_size;
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir))) 
	}
	current_process_context->process_type = USERSPACE_PROCESS;

	frame_size = 4*(argc+4)+data_size;
	frame_size += 16; //pad
	stack_pointer = USER_STACK - frame_size;

	stack_data_pointers = stack_pointer+4;
	stack_data = stack_data_pointers+argc;

	*(stack_pointer + 0) = NULL;
	*(stack_pointer + 1) = argc;
	*(stack_pointer + 2) = stack_data_pointers;
	*(stack_pointer +3 ) = NULL;
	
	z = k = j = 0;
	for(i = 0;i < argc;i++)
	{
		k = 0;
		while(bk_area[i][k] != NULL)
		{
			stack_data[j++] = bk_area[i][k];
			k++;
		}
		stack_data[j++] = '\0';
		*(stack_data_pointers+i) = ((u32) stack_data) + z;
		z = j;
	}

	for(k = 0;k < argc;k++)
	{
		kfree(bk_area[k]);
	}
	kfree(bk_area);
	SWITCH_TO_USER_MODE(stack_pointer)
	return 0;
}

void _sleep_time(unsigned int time)
{
	struct t_process_context* current_process;
	t_llist* sleep_wait_queue;

	SAVE_IF_STATUS	
	CLI 
	sleep_wait_queue=system.process_info->sleep_wait_queue;
	current_process=system.process_info->current_process->val;
	current_process->assigned_sleep_time=time;
	ll_prepend(sleep_wait_queue,current_process);	
	_sleep();
	RESTORE_IF_STATUS
}
