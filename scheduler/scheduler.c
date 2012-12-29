#include "general.h"
#include "system.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "asm.h"
#include "klib/printk.h"

//extern struct t_process_info process_info;
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

void schedule(struct t_processor_reg *processor_reg)
{
	struct t_process_context* current_process_context;
	struct t_process_context* new_process_context;
	t_llist_node* current_node;
	t_llist_node* sentinel_node;

	//FIND NEXT PROCESS TO SCHEDULE IS TRICK BECAUSE ll_next TAKE IN
	//COUNT SENTINEL NODE TOO.BEST APPROCH TO MANAGE ROUND ROBIN IS 
	//CIRCULAR LIST WITHOUT SENTINEL NODE.  
	//printk("schedule \n");
	current_process_context=system.process_info.current_process->val;
	current_node=ll_next(system.process_info.current_process);
	sentinel_node=ll_sentinel(system.process_info.process_context_list);
	if (current_node==sentinel_node)
	{
		current_node=ll_next(current_node);
	}
	new_process_context=current_node->val;
	if (current_process_context->pid!=new_process_context->pid)
	{
		do_context_switch(current_process_context,processor_reg,new_process_context);	
		system.process_info.current_process=current_node;
		//printk("context switch from %d \n",&current_process_context->pid);
		//printk("context switch to %d \n",&new_process_context->pid);
	}
}

void _sleep(struct t_processor_reg* processor_reg)
{
	struct t_process_context* current_process;
//--	SAVE_IF_STATUS
	unsigned int if_status;          
	asm("push %eax;");                       
	asm("pushfl;");                          
	asm("movl (%esp),%eax;");                
	asm("andl $0x200,%eax;");	                                                 
        asm("movl %%eax,%0;":"=r"(if_status));
	asm("popfl;");                           
	asm("pop %eax;");                         
	CLI
	current_process=system.process_info.current_process->val;
	t_llist_node* current_node=system.process_info.current_process;
	schedule(processor_reg);
	ll_delete_node(current_node);
	asm("push %eax;");
	asm("pushfl;");                          
	asm("movl %0,%%eax;"::"r"(if_status)); 
	asm("orl %eax,(%esp);");	         
	asm("popfl;");			         
	asm("pop %eax;");	
//--	RESTORE_IF_STATUS 
 
}

void _awake(struct t_process_context *new_process,struct t_processor_reg *processor_reg)
{
	t_llist_node* new_node;

	SAVE_IF_STATUS
	CLI 
	do_context_switch(system.process_info.current_process->val,processor_reg,new_process);
	new_node=ll_prepend(system.process_info.process_context_list,new_process);	
	system.process_info.current_process=new_node;
	RESTORE_IF_STATUS
}

_pause(struct t_processor_reg* processor_reg)
{
	struct t_process_context* current_process;
	t_llist* pause_queue;

	SAVE_IF_STATUS	
	CLI 
	pause_queue=system.process_info.pause_queue;
	current_process=system.process_info.current_process->val;
	ll_prepend(pause_queue,current_process);	
	_sleep(processor_reg);
	RESTORE_IF_STATUS
}

void _exit(int status,struct t_processor_reg* processor_reg)
{
	t_llist_node* next;
	t_llist_node* sentinel;
	unsigned int awake_process=0;
	struct t_process_context* current_process;
	struct t_process_context* next_process;
	
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
	kfree(FROM_PHY_TO_VIRT(current_process->phy_add_space));
	kfree(current_node->val);
	ll_delete_node(current_node);
	
	sentinel=ll_sentinel(system.process_info.pause_queue);
	next=ll_first(system.process_info.pause_queue);
	next_process=next->val;
	//IF PARENT PROCESS SLEEP AWAKE OTHERWISE SCHEDULE
	if (next==sentinel) schedule(processor_reg);
	while(next!=sentinel && !awake_process)
	{	
		if (next_process->pid==current_process->parent->pid)
		{
			awake_process=1;
		}
		if (awake_process)
		{
			_awake(next->val,processor_reg);
			ll_delete_node(next);
		}
		next=ll_next(next);
		next_process=next->val;
	}
	RESTORE_IF_STATUS
}

int _fork(struct t_processor_reg processor_reg) 
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
	struct t_process_context *parent_process_context;
	child_process_context=kmalloc(sizeof(struct t_process_context));
	CLI
	parent_process_context=system.process_info.current_process->val;
	kmemcpy(child_process_context,parent_process_context,sizeof(struct t_process_context));
	child_process_context->pid=system.process_info.next_pid++;
	child_process_context->processor_reg=processor_reg;
	child_process_context->parent=parent_process_context;
	mem_size=parent_process_context->phy_space_size;
	proc_mem=kmalloc(mem_size);
	child_process_context->phy_add_space=FROM_VIRT_TO_PHY(proc_mem);
	kmemcpy(proc_mem,FROM_PHY_TO_VIRT(parent_process_context->phy_add_space),mem_size);
	ll_prepend(system.process_info.process_context_list,child_process_context);
	child_process_context->page_dir=init_vm_process(system.master_page_dir,child_process_context->phy_add_space,child_process_context);
      	STI
	return child_process_context->pid;
}

void _exec(unsigned int start_addr,unsigned int size) 
{
	struct t_process_context *current_process_context;
	char* process_storage;
	char* process_space;
	unsigned int i=0;
	static unsigned int old_proc_phy_addr;
	static unsigned int pad;
	static void* old_page_dir;
	CLI
	current_process_context=system.process_info.current_process->val;
	current_process_context->phy_space_size=size;
	process_space=kmalloc(size);
	process_storage=FROM_PHY_TO_VIRT(start_addr);
	old_page_dir=current_process_context->page_dir;
	old_proc_phy_addr=current_process_context->phy_add_space;
	pad=current_process_context->page_pad[1024];
	current_process_context->phy_add_space=FROM_VIRT_TO_PHY(process_space);
	for (i=0;i<size;i++)
	{
		*process_space++=*process_storage++;
	}
	current_process_context->page_dir=init_vm_process(system.master_page_dir,current_process_context->phy_add_space,current_process_context);
	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir)))
	free_vm_process(old_page_dir,pad);
	kfree(FROM_PHY_TO_VIRT(old_proc_phy_addr));
        STI                                 	
	SWITCH_TO_USER_MODE
}






