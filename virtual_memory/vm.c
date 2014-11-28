#include "general.h"
#include "idt.h"
#include "memory_manager/general.h"
#include "virtual_memory/vm.h"

void page_fault_handler();

void* init_virtual_memory()
{
	unsigned int i;	
	unsigned int pad;
	unsigned int* new_page_dir;
	system.process_info->current_process=NULL;
	static struct t_i_desc i_desc;

	i_desc.baseLow=((int)&page_fault_handler) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00;
	i_desc.baseHi=((int)&page_fault_handler)>>0x10;
	set_idt_entry(0x0E,&i_desc);
	
	new_page_dir=buddy_alloc_page(system.buddy_desc,0x1000);
	for (i=0;i<1024;i++) new_page_dir[i]=0;
	map_vm_mem(new_page_dir,0,0,0x100000);
	map_vm_mem(new_page_dir,VIRT_MEM_START_ADDR,PHY_MEM_START_ADDR,(VIRT_MEM_END_ADDR-VIRT_MEM_START_ADDR));
	return new_page_dir;
}

void* __init_vm_process(struct t_process_context* process_context)
{
	unsigned int* page_dir;	
	unsigned int start,end;
	unsigned int pad;
	unsigned int i=0;

	page_dir=buddy_alloc_page(system.buddy_desc,0x1000);
	for (i=0;i<768;i++) 
	{
		page_dir[i]=0;
	}
	for (i=768;i<1024;i++) 
	{
		page_dir[i]=((unsigned int*)system.master_page_dir)[i];
	}
	map_vm_mem(page_dir,0,0,0x100000);
	if (process_context->phy_add_space!=NULL)
	{
		map_vm_mem(page_dir,PROC_VIRT_MEM_START_ADDR,process_context->phy_add_space,process_context->phy_space_size);
		map_vm_mem(page_dir,USER_STACK,process_context->phy_user_stack,0x10000);
	}
	map_vm_mem(page_dir,KERNEL_STACK,process_context->phy_kernel_stack,KERNEL_STACK_SIZE);	
	return page_dir;
}

void init_vm_process(struct t_process_context* process_context)
{
	unsigned int* page_dir;	

	page_dir=process_context->process_context;
	for (i=0;i<768;i++) 
	{
		page_dir[i]=0;
	}
	for (i=768;i<1024;i++) 
	{
		page_dir[i]=((unsigned int*)system.master_page_dir)[i];
	}
	map_vm_mem(page_dir,KERNEL_STACK,process_context->phy_kernel_stack,KERNEL_STACK_SIZE);	
}

void clone_vm_process(void* parent_page_dir)
{
	unsigned int i,j;
	unsigned int* child_page_dir;
	unsigned int* child_page_table;
	unsigned int* parent_page_table;


	child_page_dir=buddy_alloc_page(system.buddy_desc,0x1000);
	for (i=0;i<768;i++) 
	{
		if (((unsigned int*)parent_page_dir)[i]!=0)
		{
			child_page_table=buddy_alloc_page(system.buddy_desc,0x1000);
			parent_page_table=((unsigned int*)parent_page_dir)[i];
				
			for (j=0;j<1024;j++)
			{
				child_page_table)[j]=parent_page_table[j] | 5;
			}
			child_page_dir[i]=child_page_table | 5;
		}
		else
		{
			child_page_dir[i]=0;
		}
	}
	for (i=768;i<1024;i++) 
	{
		child_page_dir[i]=((unsigned int*) parent_page_dir)[i];
	}
}

void _free_vm_process(struct t_process_context* process_context)
{
	umap_vm_mem(process_context->page_dir,0,0x100000,0);
	if (process_context->phy_add_space!=NULL)
	{
		umap_vm_mem(process_context->page_dir,PROC_VIRT_MEM_START_ADDR,process_context->phy_space_size,1);
		umap_vm_mem(process_context->page_dir,USER_STACK,0x10000,1);
	}
	umap_vm_mem(process_context->page_dir,KERNEL_STACK,0x4000,1);
	buddy_free_page(system.buddy_desc,process_context->page_dir);
}

void free_vm_process(void* page_dir)
{
	unsigned int i,j;
	unsigned int* page_table;

	for (i=0;i<768;i++) 
	{
		if (((unsigned int*)page_dir)[i]!=0)
		{
			page_table=((unsigned int*)page_dir)[i];
				
			for (j=0;j<1024;j++)
			{
				if (page_table[j]!=0)
				{
					buddy_free_page(page_table[j]);
				}	
			}
			buddy_free_page(page_table);
			((unsigned int*)page_dir)[i]=0;
		}
	}
}

void map_vm_mem(void* page_dir,unsigned int vir_mem_addr,unsigned int phy_mem_addr,int mem_size)
{
	unsigned int *page_table;
	unsigned int pad;
	unsigned int i,j;
	unsigned int start,end;
	unsigned int page_count;
	unsigned int pd_count;
	unsigned int first_pd;
	unsigned int first_pt;
	unsigned int last_pt;
	unsigned int tot_pd;
	
	phy_mem_addr-=4096;
	page_count=mem_size/4096;
	if ((mem_size % 4096)>0) page_count++;
	pd_count=page_count/1024;
	if ((page_count % 1024)>0) pd_count++;
	first_pd=vir_mem_addr>>22;
	first_pt=(vir_mem_addr & 0x3FFFFF)>>12;
	last_pt=((vir_mem_addr+mem_size-1) & 0x3FFFFF)>>12;
	tot_pd=pd_count+first_pd;

	for (i=first_pd;i<tot_pd;i++)
	{
		if (((int*)(page_dir))[i]==0)
		{	
			page_table=buddy_alloc_page(system.buddy_desc,0x1000);
			for (j=0;j<1024;j++)
			{
				page_table[j]=0;
			}
			((unsigned int*)page_dir)[i]=FROM_VIRT_TO_PHY((unsigned int)page_table) | 7;
		}
		else 
		{
			page_table=FROM_PHY_TO_VIRT(((unsigned int*)page_dir)[i]) & 0xFFFFF000;
			
		}
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
			start=0;USERSPACE_PROCESS
			end=1024;
		}
		
		for (j=start;j<end;j++)
		{
			phy_mem_addr=(phy_mem_addr+4096) | 7;
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
		page_table=FROM_PHY_TO_VIRT(((unsigned int*)page_dir)[i]) & 0xFFFFF000;   

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
	u32 ustack_pointer;
	u32 fault_addr;
	u32 fault_code;
	u32 page_num;
	u32 page_offset;
	u32 stack_reg_start;
	struct t_process_context* current_process_context;
	struct t_processor_reg processor_reg;

	SAVE_PROCESSOR_REG
	GET_FAULT_ADDRESS(fault_addr,fault_code);
	CURRENT_PROCESS_CONTEXT(current_process_context);

	if (CHECK_MEM_REG(fault_addr,current_process_context->process_mem_reg))
	{

	}
	else if (CHECK_MEM_REG(fault_addr,current_process_context->heap_mem_reg))
	{
		x

	}
	else if (CHECK_MEM_REG(fault_addr,current_process_context->ustack_mem_reg))
	{
		on_exit_action=0;
		GET_STACK_POINTER(ustack_pointer)
		page_num=fault_addr / PAGE_SIZE;
		page_offset=fault_addr % PAGE_SIZE;

		if ((ustack_pointer-16)>fault_addr)
		{
			printk("\n Segmentation fault. \n");
			_exit(0);
			on_exit_action=2;
		}
		else
		{
			stack_reg_start=current_process_context->ustack_mem_reg->start_addr;-----qui
			page_addr=buddy_alloc_page(system.buddy_desc,0x1000);
			map_vm_mem(current_process_context->page_dir,(fault_addr && 0x1000),page_addr,0x1000);
			stack_reg_upper_limit=current_process_context->ustack_mem_reg->start_addr;
			if ((ustack_pointer-stack_reg_upper_limit)<USER_STACK_ENLARGE_THRESHOLD)
			{
				page_addr=buddy_alloc_page(system.buddy_desc,USER_STACK_ENLARGE_THRESHOLD);---qui
				map_vm_mem(current_process_context->page_dir,(fault_addr && 0x1000),page_addr,0x1000);
			}
		}
	}
	else
	{
		printk("\n Segmentation fault. \n");
		_exit(0);
		on_exit_action=2;
	}
	ENABLE_PREEMPTION
	EXIT_INT_HANDLER(on_exit_action,processor_reg)
}
