#include "memory_manager/kmalloc.h"
#include "asm.h"
#include "memory_manager/fixed_size.h"
#include "memory_manager/general.h"
#include "virtual_memory/vm.h"
#include "debug.h"

extern unsigned int collect_mem;
extern unsigned int collected_mem[15000];
extern unsigned int collected_mem_index;
extern unsigned int allocated_block;

t_a_fixed_size_desc a_fixed_size_desc[POOL_NUM];
unsigned int free_mem_list[POOL_NUM];

void init_kmalloc() 
{
	unsigned int i;
	void* mem_addr;

	mem_addr=POOL_START_ADDR + VIRT_MEM_START_ADDR - MEM_TO_POOL;
	//ALLOCATED POOL OF 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 BYTE
	for (i=0;i<POOL_NUM;i++)
	{
		mem_addr+=MEM_TO_POOL;
		u32 www=pow2(2+i);
		a_fixed_size_init(&a_fixed_size_desc[i],pow2(2+i),mem_addr,MEM_TO_POOL);  
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

void  *_bigMalloc(unsigned int mem_size) 
{
	return buddy_alloc_page(system.buddy_desc,mem_size);
}

void  _bigFree(void *address) 
{
	return buddy_free_page(system.buddy_desc,address);
}

void* kmalloc(unsigned int mem_size) 
{		
	int i;	
	void *mem_add;
	SAVE_IF_STATUS
	CLI	
	//SPINLOCK_LOCK
	for (i=0;i<POOL_NUM;i++)
	{
		if (mem_size<=pow2(2+i)) break;
	}
	if (i!=POOL_NUM) 
	{
		mem_add=a_fixed_size_alloc(&a_fixed_size_desc[i]);
	}
	else 
	{
		panic();
	}

	if (1)
	//if (collect_mem==1) 
	{
		collect_mem_alloc(mem_add);
		if (mem_add==0xc1da375a) 
		{
			printk("ss \n");
		}
	}	

	RESTORE_IF_STATUS
	return mem_add;
}

void kfree(void *address) 
{	
	int i;
	unsigned int pool_index;

	SAVE_IF_STATUS
	CLI	
	
	if (1) 
	//if (collect_mem==1) 
	{
		collect_mem_free(address);
		if (address==0xc1da375a) 
		{
			printk("ssr \n");
		}
	}

	pool_index=0;
	while ((pool_index+1)*MEM_TO_POOL<(address-VIRT_MEM_START_ADDR-POOL_START_ADDR))
	{
		pool_index++;
	}
	a_fixed_size_free(&a_fixed_size_desc[pool_index],address);
	
	RESTORE_IF_STATUS
}

unsigned int kfree_mem()
{
	int i;
	unsigned int tot;

	tot=0;
	for (i=0;i<POOL_NUM;i++)
	{
		free_mem_list[i]=a_fixed_size_desc[i].current_free_block;
		tot+=a_fixed_size_desc[i].current_free_block;
	}
	return tot;
}
