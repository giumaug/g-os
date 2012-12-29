//AT THE MOMENT BUDDY IS NOT IMPLEMENTED.
//MEMORY MANAGER RELIES ON FIXED_SIZE_ALLOCATOR ONLY.

#include "virtual_memory/vm.h"
#include "memory_manager/buddy.h"

t_mem_desc mem_desc;

void buddy_init_mem()
{
	int i;
	for (i=0;i<MEM_PAGE;i++) mem_desc.page_desc[i].page_add=i*PAGE_SIZE+MEM_START_ADDR+VIRT_MEM_START_ADDR;
	int rr=MEM_PAGE;
	mem_desc.index=-1;
}

void *buddy_alloc_mem(unsigned int owner_id)
{
	mem_desc.index++;
	mem_desc.page_desc[mem_desc.index].owner_id=owner_id;	
	return mem_desc.page_desc[mem_desc.index].page_add;
}

void buddy_free_mem()
{
}
