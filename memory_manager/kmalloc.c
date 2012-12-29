#include "memory_manager/buddy.h"
#include "memory_manager/mem_proc.h"
#include "memory_manager/fixed_size.h"
#include "virtual_memory/vm.h"
#include "asm.h"
#include "synchro_types/spin_lock.h"
#include "general.h"
#include "lib/math.h"
#include "memory_manager/kmalloc.h"

extern t_mem_desc mem_desc;
t_a_fixed_size_desc a_fixed_size_desc[11];

void init_kmalloc() 
{
	unsigned int i;
	buddy_init_mem();
	//TEMPORAY ALLOCATOR FOR PROCESS MEM	
	init_mem_proc();
	//ALLOCATED POOL OF 32 64 128 256 512 1024 2048 4096 8192 16384 BYTE
	for (i=0;i<10;i++)
	{
		a_fixed_size_init(&a_fixed_size_desc[i],pow2(5+i),INIT_FREE_BLOCK(pow2(5+i))); 
	}
}

void  *_malloc(unsigned int mem_size) 
{
	return kmalloc(mem_size);
}

void  _free(void *address) 
{
	return kfree(address);
}

void* kmalloc(unsigned int mem_size) 
{		
	int i;	
	void *mem_add;
	SAVE_IF_STATUS
	CLI	
	SPINLOCK_LOCK
	if (mem_size==0x100000)
	{
		return alloc_mem_proc();
	}
	for (i=0;i<10;i++)
	{
		if (mem_size<=pow2(5+i)) break;
	}
	//MAX mem_size ALLOWED 16384 BYTE
	if (i!=10) 
	{	
		mem_add=a_fixed_size_alloc(&a_fixed_size_desc[i]); //WHAT IF NO MEMORY AVAILABLE?
		//a_fixed_size_dump(&a_fixed_size_desc[0]);
	}
	SPINLOCK_UNLOCK
	RESTORE_IF_STATUS
	return mem_add;
}

void kfree(void *address) 
{
	unsigned int xx;	
	unsigned int pool_index;
	SAVE_IF_STATUS
	CLI	
	SPINLOCK_LOCK
	xx=(unsigned int) FROM_VIRT_TO_PHY(address);
	if (FROM_VIRT_TO_PHY(address)>=0xD00000 && FROM_VIRT_TO_PHY(address)<=0x1600000)
	{
		return free_mem_proc(address);
	}
	pool_index=(mem_desc.page_desc[MEM_INDEX((unsigned int)address)].owner_id)-5;
	a_fixed_size_free(&a_fixed_size_desc[pool_index],address);
	SPINLOCK_UNLOCK
	RESTORE_IF_STATUS
}




