#include "general.h"
#include "memory_manager/general.h"
#include "virtual_memory/vm.h"

struct t_process_context* process0;

void* init_virtual_memory()
{
	unsigned int i;	
	unsigned int pad;
	unsigned int* new_page_dir;
	system.process_info->current_process=NULL;
	
	new_page_dir=buddy_alloc_page(system.buddy_desc,0x1000);
	for (i=0;i<1024;i++) new_page_dir[i]=0;
	map_vm_mem(new_page_dir,0,0,0x100000);
	map_vm_mem(new_page_dir,VIRT_MEM_START_ADDR,PHY_MEM_START_ADDR,(VIRT_MEM_END_ADDR-VIRT_MEM_START_ADDR));
	return new_page_dir;
}

void* init_vm_process(void* master_page_dir,unsigned int proc_phy_addr,struct t_process_context* process_context,unsigned int flags)
{
	unsigned int *page_dir;	
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
		page_dir[i]=((unsigned int*)master_page_dir)[i];
	}
	map_vm_mem(page_dir,0,0,0x100000);
	if (flags==INIT_VM_USERSPACE)
	{
		map_vm_mem(page_dir,PROC_VIRT_MEM_START_ADDR,proc_phy_addr,0x100000);
	}
	else
	{
		map_vm_mem(page_dir,KERNEL_THREAD_STACK,process_context->phy_k_thread_stack,0x10000);	
	}
	return page_dir;
}

void free_vm_process(void* page_dir,unsigned int flags)
{
	umap_vm_mem(page_dir,0,0x100000,0);
	if (flags==INIT_VM_USERSPACE)
	{
		umap_vm_mem(page_dir,PROC_VIRT_MEM_START_ADDR,0x100000,1);
	}
	else 
	{
		umap_vm_mem(page_dir,KERNEL_THREAD_STACK,0x10000,1);
	}
	buddy_free_page(system.buddy_desc,page_dir);
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
			start=0;
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
