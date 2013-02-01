#include "general.h"
#include "asm.h"
#include "lib/math.h" //ERROR USING USER SPACE LIB !!!!!!!!!!!!!!!
#include "synchro_types/spin_lock.h"
#include "memory_manager/fixed_size.h"
#include "memory_manager/general.h"
#include "virtual_memory/vm.h"
#include "memory_manager/kmalloc.h"

 t_a_fixed_size_desc a_fixed_size_desc[POOL_NUM];

void init_kmalloc() 
{
	unsigned int i;
	void* mem_addr;

	mem_addr=POOL_START_ADDR + VIRT_MEM_START_ADDR - MEM_TO_POOL;
	//ALLOCATED POOL OF 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 BYTE
	for (i=0;i<POOL_NUM;i++)
	{
		mem_addr+=MEM_TO_POOL;
		a_fixed_size_init(&a_fixed_size_desc[i],pow2(2+i),mem_addr,MEM_TO_POOL);  
		if (i==11) 
		{
			a_fixed_size_dump();
		}
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
	for (i=0;i<POOL_NUM;i++)
	{
		if (mem_size<=pow2(2+i)) break;
	}
	if (i!=POOL_NUM) 
	{	
		if (i==11) 
		{
			a_fixed_size_dump();
		}
		mem_add=a_fixed_size_alloc(&a_fixed_size_desc[i]);
		if (i==11) 
		{
			a_fixed_size_dump();
		}
		if (mem_add==0xc23a1511)
		{
			a_fixed_size_dump();
		}
	}
	SPINLOCK_UNLOCK
	RESTORE_IF_STATUS
	return mem_add;
}

void kfree(void *address) 
{	
	unsigned int pool_index;

	SAVE_IF_STATUS
	CLI	
	SPINLOCK_LOCK
	pool_index=0;
	while ((pool_index+1)*MEM_TO_POOL<(address-VIRT_MEM_START_ADDR-POOL_START_ADDR))
	{
		pool_index++;
	}
	a_fixed_size_free(&a_fixed_size_desc[pool_index],address);
	if (pool_index==11) 
	{
		a_fixed_size_dump();
	}
	if (address==0xc23a54c4 || address==0xc23a54a4)
	{
		a_fixed_size_dump();
	}
	SPINLOCK_UNLOCK
	RESTORE_IF_STATUS
}




