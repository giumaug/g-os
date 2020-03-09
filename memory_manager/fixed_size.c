#include "memory_manager/fixed_size.h"

static int req_count=0;
static void *dump_fx[100];
extern t_a_fixed_size_desc a_fixed_size_desc[POOL_NUM];

static void a_fixed_size_reset_block(void* address,unsigned int block_size);
static void a_fixed_size_init_mem(t_a_fixed_size_desc *a_fixed_size_desc);

void a_fixed_size_init(t_a_fixed_size_desc *a_fixed_size_desc,unsigned int block_size,void* mem_addr,int size) 
{
        int index;
	unsigned int num_block;
	t_block_desc *first_block_desc;	
	t_block_desc *current_block_desc;
	t_block_desc *previous_block_desc;
	t_block_desc *next_block_desc;

	num_block=size/(sizeof(t_block_desc)+block_size); 
	a_fixed_size_desc->first_block=mem_addr;
	first_block_desc=a_fixed_size_desc->first_block;
	a_fixed_size_desc->has_initialized =1;
	a_fixed_size_desc->block_size=block_size;
	a_fixed_size_desc->current_free_block=num_block;
	
	previous_block_desc=NULL;
	current_block_desc=a_fixed_size_desc->first_block;
	for (index=1;index<num_block;index++)
        {	
                next_block_desc=((char *)current_block_desc)+sizeof(t_block_desc)+a_fixed_size_desc->block_size;
		current_block_desc->next_block=next_block_desc;
		current_block_desc->previous_block=previous_block_desc;
                previous_block_desc=current_block_desc;
                current_block_desc=next_block_desc;
        }
	current_block_desc->next_block=first_block_desc;
	current_block_desc->previous_block=previous_block_desc;
	first_block_desc->previous_block=current_block_desc;
	//a_fixed_size_init_mem(a_fixed_size_desc);
}

void *a_fixed_size_alloc(t_a_fixed_size_desc *a_fixed_size_desc) 
{
//	if (a_fixed_size_desc->block_size == 131072)
//	{
//		printk("size is %d \n",a_fixed_size_desc->current_free_block);
//		if (a_fixed_size_desc->current_free_block == 7)
//		{	req_count++;
//			printk("leak!!! %d \n",req_count);
//		}
//	}
	
	if ((a_fixed_size_desc->current_free_block) == 0)
	{
		printk("run out of kmalloc memory!!!");
		panic();
	}
        t_block_desc *block_add;
        t_page_desc *page_desc;
	t_block_desc *current_block_desc;
        t_block_desc *next_block_desc;
        t_block_desc *previous_block_desc;
        current_block_desc=a_fixed_size_desc->first_block;
        next_block_desc=current_block_desc->next_block;
	if (next_block_desc!=NULL) next_block_desc->previous_block=NULL;
        a_fixed_size_desc->first_block=next_block_desc;
        block_add=current_block_desc+1;
        (a_fixed_size_desc->current_free_block)--;
        return block_add;
}

void a_fixed_size_free(t_a_fixed_size_desc *a_fixed_size_desc,void *address) 
{
	int index;
	t_block_desc *block_desc;
	t_block_desc *first_block_desc;
	t_block_desc *current_block_desc;
        t_block_desc *next_block_desc;
	t_block_desc *previous_block_desc;
	block_desc=(char *)address-sizeof(t_block_desc);
	a_fixed_size_desc->current_free_block++;
	current_block_desc=address-sizeof(t_block_desc);
	first_block_desc=a_fixed_size_desc->first_block;
	a_fixed_size_desc->first_block=current_block_desc;
	current_block_desc->next_block=first_block_desc;
	current_block_desc->previous_block=NULL;
	first_block_desc->previous_block=current_block_desc;
}

static void a_fixed_size_init_mem(t_a_fixed_size_desc *a_fixed_size_desc)
{
	int i=0;
	int y=0;
	unsigned char* address;
	int k;
	

	t_block_desc *current_block_desc;

	current_block_desc=a_fixed_size_desc->first_block;
	address=current_block_desc+1;
	for (i=0;i<a_fixed_size_desc->current_free_block;i++)
	{
		if (a_fixed_size_desc->block_size==2048)
		{
			k=1;
		}
		for (y=0;y<a_fixed_size_desc->block_size;y++)
		{
			
			*(address+y)=0xFF;
		}
		current_block_desc=current_block_desc->next_block;
		address=current_block_desc+1;
	}
}

void a_fixed_size_check_mem_status()
{
	int i;
	int y;
	int z;
	t_a_fixed_size_desc* _a_fixed_size_desc;
	t_block_desc* current_block_desc;
	unsigned char* address;

	for(i=0;i<POOL_NUM;i++)
	{
		_a_fixed_size_desc=&a_fixed_size_desc[i];
		current_block_desc=_a_fixed_size_desc->first_block;
		address=current_block_desc+1;
		for (y=0;y<_a_fixed_size_desc->current_free_block;y++)
		{
			for (z=0;z<a_fixed_size_desc->block_size;z++)
			{
				if (*(address+z)!=0xFF)
				{
					panic();
				}
			}
			current_block_desc=current_block_desc->next_block;
			address=current_block_desc+1;
		}
	}
}

static void a_fixed_size_reset_block(void* address,unsigned int block_size)
{
	int i;
	for (i=0;i<block_size;i++)
	{
		*(unsigned char*)(address+i)=0xFF;
	}
}
