#include "malloc.h"

static t_a_usr_space_desc a_fixed_size_desc[POOL_NUM];

//Global static not initialized variable goes on .bss.It up to loader initialize it to zero.
//Global static initialized variable goes on .data.
static int malloc_initialized=-1;

static void a_usr_space_init(t_a_usr_space_desc *a_fixed_size_desc,unsigned int block_size,void* mem_addr,int size) 
{
        int index;
	unsigned int num_block;
	t_us_block_desc *first_block_desc;	
	t_us_block_desc *current_block_desc;
	t_us_block_desc *previous_block_desc;
	t_us_block_desc *next_block_desc;

	num_block=size/(sizeof(t_us_block_desc)+block_size); 
	a_fixed_size_desc->first_block=mem_addr;
	first_block_desc=a_fixed_size_desc->first_block;
	a_fixed_size_desc->has_initialized =1;
	a_fixed_size_desc->block_size=block_size;
	a_fixed_size_desc->current_free_block=num_block;
	
	previous_block_desc=NULL;
	current_block_desc=a_fixed_size_desc->first_block;
	for (index=1;index<num_block;index++)
        {	
                next_block_desc=((char *)current_block_desc)+sizeof(t_us_block_desc)+a_fixed_size_desc->block_size;
		current_block_desc->next_block=next_block_desc;
		current_block_desc->previous_block=previous_block_desc;
                previous_block_desc=current_block_desc;
                current_block_desc=next_block_desc;
        }
	current_block_desc->next_block=first_block_desc;
	current_block_desc->previous_block=previous_block_desc;
	first_block_desc->previous_block=current_block_desc;
}

static void *a_usr_space_alloc(t_a_usr_space_desc *a_fixed_size_desc) 
{
        t_us_block_desc *block_add;
        t_us_page_desc *page_desc;
	t_us_block_desc *current_block_desc;
        t_us_block_desc *next_block_desc;
        t_us_block_desc *previous_block_desc;
        current_block_desc=a_fixed_size_desc->first_block;
        next_block_desc=current_block_desc->next_block;
	if (next_block_desc!=NULL) next_block_desc->previous_block=NULL;
        a_fixed_size_desc->first_block=next_block_desc;
        block_add=current_block_desc+1;
        (a_fixed_size_desc->current_free_block)--;
        return block_add;
}

static void a_usr_space_free(t_a_usr_space_desc *a_fixed_size_desc,void *address) 
{
	int index;
	t_us_block_desc *block_desc;
	t_us_block_desc *first_block_desc;
	t_us_block_desc *current_block_desc;
        t_us_block_desc *next_block_desc;
	t_us_block_desc *previous_block_desc;

	block_desc=(char *)address-sizeof(t_us_block_desc);
	a_fixed_size_desc->current_free_block++;
	current_block_desc=address-sizeof(t_us_block_desc);
	first_block_desc=a_fixed_size_desc->first_block;
	a_fixed_size_desc->first_block=current_block_desc;
	current_block_desc->next_block=first_block_desc;
	current_block_desc->previous_block=NULL;
	first_block_desc->previous_block=current_block_desc;
}

static void init_malloc() 
{
	unsigned int i;
	void* mem_addr;

	mem_addr=HEAP_VIRT_MEM_START_ADDR-MEM_TO_POOL;
	//ALLOCATED POOL OF 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 BYTE
	for (i=0;i<POOL_NUM;i++)
	{
		mem_addr+=MEM_TO_POOL;
		a_usr_space_init(&a_fixed_size_desc[i],pow2(2+i),mem_addr,MEM_TO_POOL);  
	}
}

void* _malloc(unsigned int mem_size) 
{		
	int i;	
	void *mem_add;

	for (i=0;i<POOL_NUM;i++)
	{
		if (mem_size<=pow2(2+i)) break;
	}
	if (i!=POOL_NUM) 
	{
		if (malloc_initialized==-1)
		{
			init_malloc();
		}
		mem_add=a_usr_space_alloc(&a_fixed_size_desc[i]);
	}
	else 
	{
		mem_add=-1;
	}
	return mem_add;
}

void _free(void *address) 
{	
	unsigned int pool_index;

	pool_index=0;
	while ((pool_index+1)*MEM_TO_POOL<(address-HEAP_VIRT_MEM_START_ADDR))
	{
		pool_index++;
	}
	a_usr_space_free(&a_fixed_size_desc[pool_index],address);
}
