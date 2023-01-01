#include "memory_manager/kmalloc.h"
//#include "asm.h"
//#include "memory_manager/fixed_size.h"
//#include "memory_manager/general.h"
//#include "virtual_memory/vm.h"
//#include "debug.h"

extern unsigned int collect_mem;
extern unsigned int collected_mem[15000];
extern unsigned int collected_mem_index;
extern unsigned int allocated_block;

t_a_fixed_size_desc a_fixed_size_desc[POOL_NUM];
unsigned int free_mem_list[POOL_NUM];
t_hashtable* aligned_address_map = NULL;

void init_kmallocs() 
{
	unsigned int i;
	void* mem_addr = NULL;

	mem_addr = POOL_START_ADDR + VIRT_MEM_START_ADDR - MEM_TO_POOL;
	//ALLOCATED POOL OF 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 BYTE
	for (i = 0; i < POOL_NUM; i++)
	{
		mem_addr += MEM_TO_POOL;
		a_fixed_size_init(&a_fixed_size_desc[i], pow2(2+i), mem_addr, MEM_TO_POOL);  
	}
	aligned_address_map = hashtable_init(ALGND_ADDR_MAP_INIT_SIZE);
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


void* _kmalloc(unsigned int mem_size) 
{		
	void *mem_add;

	SAVE_IF_STATUS
	CLI
	switch(mem_size)
	{
		case 0 ... 4:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[0]);
			break;
		}
		case 5 ... 8:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[1]);
			break;
		}
		case 9 ... 16:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[2]);
			break;
		}
		case 17 ... 32:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[3]);
			break;
		}
		case 33 ... 64:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[4]);
			break;
		}
		case 65 ... 128:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[5]);
			break;
		}
		case 129 ... 256:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[6]);
			break;
		}
		case 257 ... 512:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[7]);
			break;
		}
		case 513 ... 1024:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[8]);
			break;
		}
		case 1025 ... 2048:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[9]);
			break;
		}
		case 2049 ... 4096:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[10]);
			break;
		}
		case 4097 ... 8192:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[11]);
			break;
		}
		case 8193 ... 16384:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[12]);
			break;
		}
		case 16385 ... 32768:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[13]);
			break;
		}
		case 32769 ... 65536:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[14]);
			break;
		}
		case 65537 ... 131072:
		{
			mem_add=a_fixed_size_alloc(&a_fixed_size_desc[15]);
			break;
		}
		default:
		{
			panic();
		}
	}
//	collect_mem_alloc(mem_add);
	RESTORE_IF_STATUS
	return mem_add;
}

void* kmalloc(unsigned int mem_size) 
{		
	int i;	
	void *mem_add;
	struct t_process_context* process_context;
	static int ss = 0;

	SAVE_IF_STATUS
	CLI	
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
	if (i == 6 && a_fixed_size_desc[i].current_free_block < 100)
	{
		panic();
	}

//	if (collect_mem == 1)
//	{
//		collect_mem_alloc(mem_add);
//	}
	RESTORE_IF_STATUS
	return mem_add;
}

void _kfree(void *address) 
{	
	int i;
	unsigned int pool_index;
	u32 pool_offset;

	SAVE_IF_STATUS
	CLI	  
	pool_index=0;

	pool_offset = address - VIRT_MEM_START_ADDR - POOL_START_ADDR;

	int mem_pool = MEM_TO_POOL;
	int pool_start = POOL_START_ADDR;

	switch(pool_offset)
	{
		case 0 ... MEM_TO_POOL - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[0],address);
			break;
		}
		case MEM_TO_POOL ... (2 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[1],address);
			break;
		}
		case (2 * MEM_TO_POOL) ... (3 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[2],address);
			break;
		}
		case (3 * MEM_TO_POOL) ... (4 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[3],address);
			break;
		}
		case (4 * MEM_TO_POOL) ... (5 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[4],address);
			break;
		}
		case (5 * MEM_TO_POOL) ... (6 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[5],address);
			break;
		}
		case (6 * MEM_TO_POOL) ... (7 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[6],address);
			break;
		}
		case (7 * MEM_TO_POOL) ... (8 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[7],address);
			break;
		}
		case (8 * MEM_TO_POOL) ... (9 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[8],address);
			break;
		}
		case (9 * MEM_TO_POOL) ... (10 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[9],address);
			break;
		}
		case (10 * MEM_TO_POOL) ... (11 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[10],address);
			break;
		}
		case (11 * MEM_TO_POOL) ... (12 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[11],address);
			break;
		}
		case (12 * MEM_TO_POOL) ... (13 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[12],address);
			break;
		}
		case (13 * MEM_TO_POOL) ... (14 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[13],address);
			break;
		}
		case (14 * MEM_TO_POOL) ... (15 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[14],address);
			break;
		}
		case (15 * MEM_TO_POOL) ... (16 * MEM_TO_POOL) - 1 :
		{
			a_fixed_size_free(&a_fixed_size_desc[15],address);
			break;
		}
		default:
		{
			panic();
		}
	}
	RESTORE_IF_STATUS
}

void kfree(void* address) 
{	
	unsigned int pool_index;

	SAVE_IF_STATUS
	CLI	  
	pool_index=0;
	while ((pool_index+1)*MEM_TO_POOL<(address-VIRT_MEM_START_ADDR-POOL_START_ADDR))
	{
		pool_index++;
	}
	a_fixed_size_free(&a_fixed_size_desc[pool_index],address);
//	if (collect_mem == 1)
//	{
//		collect_mem_free(address);
//	}
	RESTORE_IF_STATUS
}

unsigned int kfree_mem()
{
	int i;
	unsigned int tot;
	int xx;

	tot=0;
	for (i=0;i<POOL_NUM;i++)
	{
		free_mem_list[i]=a_fixed_size_desc[i].current_free_block;
//		if (i == 1)
//		{
//			xx = a_fixed_size_desc[i].current_free_block;
//			printk("single pool= %d \n",xx);
//		}
		tot+=a_fixed_size_desc[i].current_free_block;
	}
	return tot;
}

void* aligned_kmalloc(u32 mem_size, u32 alignment)
{
	void* addr = NULL;
	void* aligned_addr = NULL;
	
	addr = kmalloc(mem_size + alignment);
    aligned_addr = ALIGNED_TO_OFFSET(addr, alignment);
    hashtable_put(aligned_address_map, aligned_addr, addr);
    return aligned_addr;
}

void aligned_kfree(void* address)
{
	void* address_orig = NULL;
	
	address_orig = hashtable_remove(aligned_address_map, address);
	kfree(address_orig);
}


