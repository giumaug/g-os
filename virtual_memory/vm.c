#include "general.h"
#include "system.h"
#include "memory_manager/general.h"
#include "virtual_memory/vm.h"

extern t_system system;

void* init_virtual_memory()
{
	unsigned int i;	
	unsigned int pad;
	unsigned int* new_page_dir;
	system.process_info.current_process=NULL;
	new_page_dir=kmalloc(8192);
	system.page_pad=kmalloc(sizeof(int)*1025);
	pad=((unsigned int)new_page_dir % 4096)!=0 ? 4096-((unsigned int)new_page_dir % 4096) : 0;
	new_page_dir=((unsigned int)new_page_dir)+pad;
	system.page_pad[1024]=pad;
	for (i=0;i<1024;i++) new_page_dir[i]=0;
	map_vm_mem(new_page_dir,0,0,0x100000,system.page_pad);
	map_vm_mem(new_page_dir,VIRT_MEM_START_ADDR,PHY_MEM_START_ADDR,(VIRT_MEM_END_ADDR-VIRT_MEM_START_ADDR),system.page_pad);
	return new_page_dir;
}

void* init_vm_process(void* master_page_dir,unsigned int proc_phy_addr,struct t_process_context* process_context)
{
	unsigned int *page_dir;	
	unsigned int start,end;
	unsigned int pad;
	unsigned int i=0;
	a_fixed_size_dump();
	page_dir=kmalloc(8192);
	a_fixed_size_dump();
	process_context->page_pad=kmalloc(sizeof(int)*1025);
	pad=((unsigned int)page_dir % 4096)!=0 ? 4096-((unsigned int)page_dir % 4096) : 0;
	page_dir=((unsigned int) page_dir) + pad;
	process_context->page_pad[1024]=pad;
	for (i=0;i<768;i++) 
	{
		page_dir[i]=0;
	}
	for (i=768;i<1024;i++) 
	{
		page_dir[i]=((unsigned int*)master_page_dir)[i];
	}
	map_vm_mem(page_dir,0,0,0x100000,process_context->page_pad);
	map_vm_mem(page_dir,PROC_VIRT_MEM_START_ADDR,proc_phy_addr,0x100000,process_context->page_pad);
	return page_dir;
}

void free_vm_process(void* page_dir,unsigned int* old_page_pad)
{
	umap_vm_mem(page_dir,0,0x100000,old_page_pad);
	umap_vm_mem(page_dir,PROC_VIRT_MEM_START_ADDR,0x100000,old_page_pad);
	kfree(page_dir-old_page_pad[1024]);
}

void map_vm_mem(void* page_dir,unsigned int vir_mem_addr,unsigned int phy_mem_addr,int mem_size,unsigned int* page_pad)
{
	unsigned int *page_table;
	void *page_addr;
	unsigned int pad;
	unsigned int i,j;
	unsigned int start,end;
	unsigned int page_count;
	unsigned int pd_count;
	unsigned int first_pd;
	unsigned int first_pt;
	unsigned int last_pt;
	unsigned int tot_pd;

//	//TO FIX WHEN BUDDY ALLOCATOR AVAILABLE
//	if (system.process_info.current_process!=NULL) 
//	{
//		page_pad=(((struct t_process_context*)system.process_info.current_process->val)->page_pad);
//	}
//	else 
//	{
//		page_pad=system.page_pad;
//	}
	
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
			//Because memory pool doesn't return aligned memory,ask for 8k
			//and pad to align to 4k.To fix when buddy allocator available. 	
			page_addr=kmalloc(8192);
			pad=((unsigned int)page_addr % 4096)!=0 ? 4096-((unsigned int)page_addr % 4096) : 0;
			page_pad[i]=pad;
			page_table=(unsigned int)page_addr+pad;
			for (j=0;j<1024;j++)
			{
				page_table[j]=0;
			}
			((unsigned int*)page_dir)[i]=FROM_VIRT_TO_PHY((unsigned int)page_table) | 7;
		}
		else 
		{
			page_table=FROM_PHY_TO_VIRT(((unsigned int*)page_dir)[i]) & 0xFFFFFFF8;??????????????
			
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

void umap_vm_mem(void* page_dir,unsigned int virt_mem_addr,unsigned int mem_size,unsigned int* page_pad)
{
	unsigned int *page_table;
	unsigned int start,end;
	unsigned int i;
	unsigned int page_count;
	unsigned int pd_count;
	unsigned int first_pd;
	unsigned int first_pt;
	unsigned int last_pt;

//	//TO FIX WHEN BUDDY ALLOCATOR AVAILABLE	
//	if (system.process_info.current_process!=NULL) 
//	{
//		page_pad=(((struct t_process_context*)system.process_info.current_process->val)->page_pad);
//	}
//	else 
//	{
//		page_pad=system.page_pad;
//	}
	
	page_count=mem_size/4096;
	if ((mem_size % 4096)>0) page_count++;
	pd_count=page_count/1024;
	if ((page_count % 1024)>0) pd_count++;
	first_pd=virt_mem_addr>>22;
	first_pt=(virt_mem_addr & 0x3FFFFF)>>12;
	last_pt=((virt_mem_addr+mem_size) & 0x3FFFFF)>>12;

	for (i=0;i<pd_count;i++)
	{	
		page_table=FROM_PHY_TO_VIRT(((unsigned int*)page_dir)[i]) & 0xFFFFFFF8;
		
		if (i==0) 
		{
			start=first_pt;
			end=1024;
		}
		else if (i==pd_count-1)
		{
			start=0;
			end=last_pt+1;
		}
		else 
		{
			start=0;
			end=1024;
		}
		if (start==0 && end==1024) 
		{
			kfree(page_table-page_pad[i]);
			//((unsigned int*)page_dir)[i]=0;
		}
		((unsigned int*)page_dir)[i]=0;
	}
}
