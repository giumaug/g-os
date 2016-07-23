/*
Consideration on how to manage race condition on atomic block.
I allow interruption (no lock) in a block (logically) atomic if the resulting corrupted state
cannot be observed abroad.
Following this rule free_vm_process_user_space needs lock because system.buddy_desc->count 
variable can be observed abroad. map_vm_mem doesn't lock because corrupted state cannot
observed abroad.
*/

#include "idt.h"
#include "asm.h"
#include "memory_manager/general.h"
#include "scheduler/process.h"
#include "virtual_memory/vm.h"

void page_fault_handler();

void* init_virtual_memory()
{
	unsigned int i;	
	unsigned int pad;
	unsigned int* new_page_dir;
	system.process_info->current_process=NULL;
	static struct t_i_desc i_desc;
	struct t_process_context* current_process_context;

	i_desc.baseLow=((int)&page_fault_handler) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00;
	i_desc.baseHi=((int)&page_fault_handler)>>0x10;
	set_idt_entry(0x0E,&i_desc);
	
	new_page_dir=buddy_alloc_page(system.buddy_desc,0x1000);
	buddy_clean_mem(new_page_dir);
	map_vm_mem(new_page_dir,0,0,0x100000,3);
	map_vm_mem(new_page_dir,VIRT_MEM_START_ADDR,PHY_MEM_START_ADDR,(VIRT_MEM_END_ADDR-VIRT_MEM_START_ADDR),3);
	return new_page_dir;
}

void init_vm_process(struct t_process_context* process_context)
{
	u32 i;
	unsigned int* page_dir;
	unsigned int* page_table;
	struct t_process_context* current_process_context;

	page_dir=process_context->page_dir;
	map_vm_mem(page_dir,0,0,0x100000,3);
	page_table=FROM_PHY_TO_VIRT(((unsigned int*)page_dir)[0]);
	
	for (i=256;i<1024;i++)
	{
		page_table[i]=0;
	}

	for (i=1;i<768;i++) 
	{
		page_dir[i]=0;
	}
	for (i=768;i<1024;i++) 
	{
		page_dir[i]=((unsigned int*)system.master_page_dir)[i];
	}
	map_vm_mem(page_dir,(KERNEL_STACK-KERNEL_STACK_SIZE),process_context->phy_kernel_stack,KERNEL_STACK_SIZE,3);
	CURRENT_PROCESS_CONTEXT(current_process_context);
	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir))) 
}

void* clone_vm_process(void* parent_page_dir,u32 process_type,u32 kernel_stack_addr)
{
	unsigned int i,j;
	unsigned int* child_page_dir;
	unsigned int* child_page_table;
	unsigned int* parent_page_table;
	char* page_addr;
	struct t_process_context* current_process_context;

	SAVE_IF_STATUS
	CLI
	child_page_dir=buddy_alloc_page(system.buddy_desc,PAGE_SIZE);
	buddy_clean_mem(child_page_dir);
	//map_vm_mem(child_page_dir,0,0,0x100000,3);
	child_page_table=buddy_alloc_page(system.buddy_desc,0x1000);
	buddy_clean_mem(child_page_table);
	//ENABLE TO ALL PAGE DIR TO MANAGE PAGE DIR WITH PAGE TABLE WITH DIFFERENT PRIVILEGES
	((unsigned int*)child_page_dir)[0]=FROM_VIRT_TO_PHY((unsigned int)child_page_table) | 7;

	parent_page_table=ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*)parent_page_dir)[0]));	
	for (j=0;j<256;j++)
	{	
		child_page_table[j] = parent_page_table[j];
	}

	map_vm_mem(child_page_dir,(KERNEL_STACK-KERNEL_STACK_SIZE),kernel_stack_addr,KERNEL_STACK_SIZE,3);
	if (process_type==USERSPACE_PROCESS)
	{
		parent_page_table=ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*)parent_page_dir)[0]));
		child_page_table=ALIGN_4K(FROM_PHY_TO_VIRT(child_page_dir[0]));
		for (j=256;j<1024;j++)
		{	
			parent_page_table[j] &= 0xFFFFFFFD;
			child_page_table[j]=parent_page_table[j];
			if (child_page_table[j]!=0)
			{
				system.buddy_desc->count[BLOCK_INDEX(ALIGN_4K(parent_page_table[j]))]++;
			}
		}

		for (i=1;i<768;i++)
		{
			if (ALIGN_4K(((unsigned int*)parent_page_dir)[i])!=0)
			{
				if (i!=767)
				{				
					child_page_table=buddy_alloc_page(system.buddy_desc,PAGE_SIZE);
					buddy_clean_mem(child_page_table);
				}
				else
				{
					child_page_table=ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*) child_page_dir)[767]));
				}
				parent_page_table=ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*)parent_page_dir)[i]));
				
				u32 _break=0;
				for (j=0;j<1024;j++)
				{
					if (i==767 && j==1019)
					{
						_break=1;
					}
					if (i!=767 || (i==767 && j!=1021 && j!=1022))
					{
						parent_page_table[j] &= 0xFFFFFFFD;
						child_page_table[j]=parent_page_table[j];
						if (child_page_table[j]!=0)
						{
							u32 tmp=BLOCK_INDEX(ALIGN_4K(parent_page_table[j]));
							system.buddy_desc->count[BLOCK_INDEX(ALIGN_4K(parent_page_table[j]))]++;
						}
					}
				}
				if (i!=767)
				{
					child_page_dir[i]=FROM_VIRT_TO_PHY(((unsigned int) child_page_table)) | 7;
				}
				if (i==767)
				{
					unsigned int kkk=(((unsigned int*)parent_page_dir)[i]) | 7;
					((unsigned int*) parent_page_dir)[i]=kkk;
				}
			}
			else
			{
				if (i!=767)
				{
					child_page_dir[i]=0;
				}
			}
		}
	}

	for (i=768;i<1024;i++) 
	{
		child_page_dir[i]=((unsigned int*) parent_page_dir)[i];
	}
	CURRENT_PROCESS_CONTEXT(current_process_context);
	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir))) 
	RESTORE_IF_STATUS
	return child_page_dir;
}

//NO SWITCH_PAGE_DIR NEEDED BECAUSE IT WORKS ON OLD PROCESS
void free_vm_process(struct t_process_context* process_context)
{
	SAVE_IF_STATUS
	CLI	
	if (process_context->process_type==USERSPACE_PROCESS)
	{
		free_vm_process_user_space(process_context);
	}
	umap_vm_mem(process_context->page_dir,0,0x100000,1);
	umap_vm_mem(process_context->page_dir,KERNEL_STACK,KERNEL_STACK_SIZE,1);
	buddy_free_page(system.buddy_desc,process_context->page_dir);
	RESTORE_IF_STATUS
}

void free_vm_process_user_space(struct t_process_context* process_context)
{
	unsigned int i,j;
	unsigned int* page_table;
	void* page_dir;

	SAVE_IF_STATUS
	CLI
	page_dir=process_context->page_dir;
	page_table=ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*)page_dir)[0]));
	
	for (i=256;i<1024;i++)
	{
		if (page_table[i]!=0 && system.buddy_desc->count[BLOCK_INDEX(ALIGN_4K(page_table[i]))]==1)
		{
			buddy_free_page(system.buddy_desc,ALIGN_4K(FROM_PHY_TO_VIRT(page_table[i])));
			system.buddy_desc->count[BLOCK_INDEX(ALIGN_4K(page_table[i]))]=0;
		}
		else if (page_table[i]!=0)
		{
			u32 tmp=BLOCK_INDEX(ALIGN_4K(page_table[i]));
			system.buddy_desc->count[BLOCK_INDEX(ALIGN_4K(page_table[i]))]--;
		}
		page_table[i]=0;
	}

	for (i=1;i<768;i++) 
	{
		if (((unsigned int*)page_dir)[i]!=0)
		{
			page_table=ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*)page_dir)[i]));
				
			for (j=0;j<1024;j++)
			{
				if (i!=767 || (i==767 && j!=1021 && j!=1022)) 
				{
					if (page_table[j]!=0 && system.buddy_desc->count[BLOCK_INDEX(ALIGN_4K(page_table[j]))]==1)
					{
						buddy_free_page(system.buddy_desc,FROM_PHY_TO_VIRT(ALIGN_4K(page_table[j])));
						system.buddy_desc->count[BLOCK_INDEX(ALIGN_4K(page_table[j]))]=0;
						page_table[j]=0;
					}
					else if (page_table[j]!=0)
					{
						system.buddy_desc->count[BLOCK_INDEX(ALIGN_4K(page_table[j]))]--;
						page_table[j]=0;
					}
				}	
			}
			if (i!=767)
			{
				buddy_free_page(system.buddy_desc,page_table);
				((unsigned int*)page_dir)[i]=0;
			}
		}
	}
	//SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) process_context->page_dir))) 
	RESTORE_IF_STATUS
}

void map_vm_mem(void* page_dir,unsigned int vir_mem_addr,unsigned int phy_mem_addr,int mem_size,u32 flags)
{
	unsigned int* page_table;
	unsigned int pad;
	unsigned int i,j;
	unsigned int start,end;
	unsigned int page_count;
	unsigned int pd_count;
	unsigned int first_pd;
	unsigned int first_pt;
	unsigned int last_pt;
//	unsigned int tot_pd;
	unsigned int last_pd;
	
	phy_mem_addr-=4096;
	page_count=mem_size/4096;
	if ((mem_size % 4096)>0) page_count++;
	pd_count=page_count/1024;
	if ((page_count % 1024)>0) pd_count++;
	first_pd=vir_mem_addr>>22;
	first_pt=(vir_mem_addr & 0x3FFFFF)>>12;
	last_pt=((vir_mem_addr+mem_size-1) & 0x3FFFFF)>>12;
	last_pd=pd_count+first_pd;

	for (i=first_pd;i<last_pd;i++)
	{
		if (((int*)(page_dir))[i]==0)
		{	
			page_table=buddy_alloc_page(system.buddy_desc,0x1000);
			buddy_clean_mem(page_table);
			//ENABLE TO ALL PAGE DIR TO MANAGE PAGE DIR WITH PAGE TABLE WITH DIFFERENT PRIVILEGES
			((unsigned int*)page_dir)[i]=FROM_VIRT_TO_PHY((unsigned int)page_table) | 7;
		}
		else 
		{
			page_table=ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*)page_dir)[i]));
			
		}
		if (i==first_pd && pd_count>1) 
		{
			start=first_pt;
			end=1024;
		}
		else if (i==first_pd && pd_count==1) 
		{
			start=first_pt;
			end=last_pt+1;
		}
		else if (i==last_pd-1)
		{
			start=0;
			end=last_pt+1;
		}
		else 
		{
			start=0;
			end=1024;
		}
		
		for (j=start;j<end;j++)
		{
			//phy_mem_addr=(phy_mem_addr+4096) | 7; 
			phy_mem_addr=(phy_mem_addr+4096) | flags;
			page_table[j]=phy_mem_addr;
		}
	}
}

void umap_vm_mem(void* page_dir,unsigned int virt_mem_addr,unsigned int mem_size,unsigned int flush)
{
	unsigned int *page_table;
	unsigned int start,end;
	unsigned int i;
	unsigned int page_count;
	unsigned int pd_count;
	unsigned int first_pd;
	unsigned int first_pt;
	unsigned int last_pt;
	unsigned int tot_pd;
	
	page_count=mem_size/4096;
	if ((mem_size % 4096)>0) page_count++;
	pd_count=page_count/1024;
	if ((page_count % 1024)>0) pd_count++;
	first_pd=virt_mem_addr>>22;
	first_pt=(virt_mem_addr & 0x3FFFFF)>>12;
	last_pt=((virt_mem_addr+mem_size-1) & 0x3FFFFF)>>12;
	tot_pd=pd_count+first_pd;

	for (i=first_pd;i<tot_pd;i++)
	{	
		page_table=ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*)page_dir)[i]));   

		if (i==first_pd && tot_pd>1) 
		{
			start=first_pt;
			end=1024;
		}
		else if (i==first_pd && tot_pd==1) 
		{
			start=first_pt;
			end=last_pt+1;
		}
		else if (i==tot_pd-1)
		{
			start=0;
			end=last_pt+1;
		}
		else 
		{
			start=0;
			end=1024;
		}

		if ((start==0 && end==1024) || flush) 
		{
			buddy_free_page(system.buddy_desc,page_table);
			((unsigned int*)page_dir)[i]=0;
		}
	}
}

void page_fault_handler()
{
	u32 on_exit_action;
	u32* ustack_pointer;
	u32 fault_addr;
	u32 aligned_fault_addr;
	char fault_code;
	u32 page_addr;
	u32 page_num;
	u32 page_offset;
	u32 stack_reg_start;
	struct t_process_context* current_process_context;
	struct t_processor_reg processor_reg;
	u32 pd_num;
	u32 pt_num;
	u32* page_table;
	u32 phy_fault_addr;
	void* parent_page_table;

	SAVE_PROCESSOR_REG
	SWITCH_DS_TO_KERNEL_MODE
	GET_FAULT_ADDRESS(fault_addr,fault_code);
	CURRENT_PROCESS_CONTEXT(current_process_context);

	on_exit_action=0;
	page_num=fault_addr / PAGE_SIZE;
	page_offset=fault_addr % PAGE_SIZE;
	aligned_fault_addr=fault_addr & (~(PAGE_SIZE-1));
	parent_page_table=current_process_context->parent->page_dir;

	PRINTK("page fault address=%d \n",fault_addr);
	PRINTK("\n");

	ustack_pointer=processor_reg.esp+20;

	if (CHECK_MEM_REG(fault_addr,current_process_context->process_mem_reg)
		    || CHECK_MEM_REG(fault_addr,current_process_context->heap_mem_reg)
		    || CHECK_MEM_REG(fault_addr,current_process_context->ustack_mem_reg))
	{
		if ((fault_code & 0x1)==PAGE_OUT_MEMORY && CHECK_MEM_REG(fault_addr,current_process_context->process_mem_reg))
		{
			page_addr=buddy_alloc_page(system.buddy_desc,PAGE_SIZE);
			map_vm_mem(current_process_context->page_dir,aligned_fault_addr,FROM_VIRT_TO_PHY(page_addr),PAGE_SIZE,7);
				
			system.buddy_desc->count[BLOCK_INDEX(FROM_VIRT_TO_PHY(page_addr))]++;
			elf_loader_read(current_process_context->elf_desc,fault_addr,page_addr);
		}
		else if ((fault_code & 0x1)==PAGE_OUT_MEMORY && (CHECK_MEM_REG(fault_addr,current_process_context->heap_mem_reg) || CHECK_MEM_REG(fault_addr,current_process_context->ustack_mem_reg)))
		{
			page_addr=buddy_alloc_page(system.buddy_desc,PAGE_SIZE);
			map_vm_mem(current_process_context->page_dir,aligned_fault_addr,FROM_VIRT_TO_PHY(page_addr),PAGE_SIZE,7);
			system.buddy_desc->count[BLOCK_INDEX(FROM_VIRT_TO_PHY(page_addr))]++;				
		}
		else if ((fault_code & 0x3)==(PAGE_IN_MEMORY | PAGE_WRITE))
		{
			pd_num=aligned_fault_addr>>22;
			pt_num=(aligned_fault_addr & 0x3FFFFF)>>12;
			page_table=ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*) current_process_context->page_dir)[pd_num]));
			phy_fault_addr=ALIGN_4K(((unsigned int*) page_table)[pt_num]);
			((unsigned int*) page_table)[pt_num] |= 7;
                                
			u32 tmp=BLOCK_INDEX(phy_fault_addr);

			if (system.buddy_desc->count[BLOCK_INDEX(phy_fault_addr)]>1)
			{
				page_addr=buddy_alloc_page(system.buddy_desc,PAGE_SIZE);
				kmemcpy(page_addr,aligned_fault_addr ,PAGE_SIZE);
				map_vm_mem(current_process_context->page_dir,aligned_fault_addr,FROM_VIRT_TO_PHY(page_addr),PAGE_SIZE,7);
				system.buddy_desc->count[BLOCK_INDEX(phy_fault_addr)]--;
				system.buddy_desc->count[BLOCK_INDEX(FROM_VIRT_TO_PHY(page_addr))]++;
			}
		}
		else
		{
			printk("ssssssssssssssssssssss\n");
		}
	}
	else if ((fault_code & 0x4)==USER && ((*(u32*)(processor_reg.esp+20))-32)<=fault_addr)
	{
		current_process_context->ustack_mem_reg->start_addr=aligned_fault_addr;
		page_addr=buddy_alloc_page(system.buddy_desc,PAGE_SIZE);
		map_vm_mem(current_process_context->page_dir,aligned_fault_addr,FROM_VIRT_TO_PHY(page_addr),PAGE_SIZE,7);
		system.buddy_desc->count[BLOCK_INDEX(FROM_VIRT_TO_PHY(page_addr))]++;
	}
	else
	{
		printk("\n ...Segmentation fault. \n");
		_exit(0);
		on_exit_action=2;
	}

	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir))) 	
//-	EXIT_INT_HANDLER(on_exit_action,processor_reg)
                                                
	static struct t_process_context _current_process_context;                                                  		
	static struct t_process_context _old_process_context;                                                      		
	static struct t_process_context _new_process_context;	                                                   		
	static struct t_processor_reg _processor_reg;                                                              		
	static unsigned int _action2;                                                                              		
                                                                                                                   		
	CLI                                                                                                        		
	_action2=on_exit_action;                                                                                           		
	_current_process_context=*(struct t_process_context*)system.process_info->current_process->val;             		
	_old_process_context=_current_process_context;                                                             		
	_processor_reg=processor_reg;
                                                       		
	if (_action2>0)                                                                                            		
	{                                                                                                          		
		schedule(&_current_process_context,&_processor_reg);                                               		
		_new_process_context=*(struct t_process_context*)system.process_info->current_process->val;         		
		_processor_reg=_new_process_context.processor_reg;                                                              
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
		RET_FROM_INT_HANDLER_FLUSH                                                                       		
	}
}
