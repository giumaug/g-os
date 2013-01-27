#include "general.h"
#include "memory_manager/general.h"
#include "memory_manager/fixed_size.h"

static void *dump_fx[100];
extern t_a_fixed_size_desc a_fixed_size_desc[POOL_NUM];

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
}

void *a_fixed_size_alloc(t_a_fixed_size_desc *a_fixed_size_desc) 
{
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

//void a_fixed_size_dump(t_a_fixed_size_desc *a_fixed_size_desc) 
void a_fixed_size_dump() 
{
	int i=0;
	static int ss;	
	t_a_fixed_size_desc *_a_fixed_size_desc=&a_fixed_size_desc[11];	
	t_block_desc *current_block_desc;
	current_block_desc=_a_fixed_size_desc->first_block;

	for (i=0;i<100;i++)
	{
		dump_fx[i]=((char*)(current_block_desc)+0xc);
		if (dump_fx[i]==0xc23a54c8)
		{
			ss++;
		}
		current_block_desc=current_block_desc->next_block;
	}
	return;
}

