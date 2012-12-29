#include "virtual_memory/vm.h"
#include "memory_manager/mem_proc.h"

t_mem_proc_desc mem_proc_desc[10];

void init_mem_proc()
{
	int i;
	for (i=0;i<10;i++)
	{
		mem_proc_desc[i].mem_addr=0xD00000+(0x100000*i);
		mem_proc_desc[i].status=0;
	}
}

void* alloc_mem_proc()
{
	int i;	
	for (i=0;i<10;i++)
	{
		if (mem_proc_desc[i].status==0)
		{
			mem_proc_desc[i].status=1;
			return FROM_PHY_TO_VIRT(mem_proc_desc[i].mem_addr);
		}
	}
}

void free_mem_proc(void* addr)
{
	int i;	
	int xxx;
	for (i=0;i<10;i++)
	{
		xxx=FROM_PHY_TO_VIRT(mem_proc_desc[i].mem_addr);
		if (FROM_PHY_TO_VIRT(mem_proc_desc[i].mem_addr)==addr)
		{
			return mem_proc_desc[i].status=0;
		}
	}
}
