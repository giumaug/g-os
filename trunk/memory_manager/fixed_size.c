#include "general.h"
#include "lib/math.h"
#include "memory_manager/kmalloc.h"
#include "memory_manager/buddy.h"
#include "memory_manager/fixed_size.h"

static void *dump_fx[100];

static void alloc_page(t_a_fixed_size_desc *a_fixed_size_desc)
{
	int index;
	t_block_desc *first_block_desc;
	t_page_desc *page_desc;
	t_block_desc *current_block_desc;
	t_block_desc *previous_block_desc;
	t_block_desc *next_block_desc;
	first_block_desc=a_fixed_size_desc->first_block;
	a_fixed_size_desc->first_block=buddy_alloc_mem(log2(a_fixed_size_desc->block_size))+sizeof(t_page_desc);
	page_desc=((char *) a_fixed_size_desc->first_block)-sizeof(t_page_desc);
	page_desc->block_free=a_fixed_size_desc->num_block;
        current_block_desc=a_fixed_size_desc->first_block;
	current_block_desc->page_desc=page_desc;
        previous_block_desc=NULL;
	for (index=1;index<a_fixed_size_desc->num_block;index++)
        {	
                next_block_desc=((char *)current_block_desc)+sizeof(t_block_desc)+a_fixed_size_desc->block_size;
		current_block_desc->next_block=next_block_desc;
		current_block_desc->previous_block=previous_block_desc;
		current_block_desc->page_desc=page_desc;
                previous_block_desc=current_block_desc;
                current_block_desc=next_block_desc;
        }
	current_block_desc->next_block=first_block_desc;
	current_block_desc->previous_block=previous_block_desc;
	current_block_desc->page_desc=page_desc;
	if (first_block_desc!=NULL) first_block_desc->previous_block=current_block_desc;
	(a_fixed_size_desc->current_free_block)+=a_fixed_size_desc->num_block;
}

void a_fixed_size_init(t_a_fixed_size_desc *a_fixed_size_desc,unsigned int block_size,unsigned int init_free_block) 
{
        int index;
	unsigned int required_page;
        unsigned int num_block=(PAGE_SIZE-sizeof(t_page_desc))/(sizeof(t_block_desc)+block_size); // to del
	a_fixed_size_desc->first_block=NULL;
	a_fixed_size_desc->has_initialized =1;
	a_fixed_size_desc->block_size=block_size;
        a_fixed_size_desc->num_block=NUM_BLOCK(block_size);
	a_fixed_size_desc->init_free_block=init_free_block;
	a_fixed_size_desc->num_page=0;
	required_page=(init_free_block%num_block)==0 ? (init_free_block/num_block) : (init_free_block/num_block)+1; // to del
	a_fixed_size_desc->current_free_block=0;
        for (index=0;index<NUM_PAGE(init_free_block,block_size);index++)
	{
        	alloc_page(a_fixed_size_desc);
		a_fixed_size_desc->num_page++;
		//a_fixed_size_dump(a_fixed_size_desc);
	}
}

void *a_fixed_size_alloc(t_a_fixed_size_desc *a_fixed_size_desc) 
{
        t_block_desc *block_add;
        t_page_desc *page_desc;
	t_block_desc *current_block_desc;
        t_block_desc *next_block_desc;
        t_block_desc *previous_block_desc;
	if (a_fixed_size_desc->first_block==NULL) alloc_page(a_fixed_size_desc);
        current_block_desc=a_fixed_size_desc->first_block;
        next_block_desc=current_block_desc->next_block;
	if (next_block_desc!=NULL) next_block_desc->previous_block=NULL;
        a_fixed_size_desc->first_block=next_block_desc;
        block_add=current_block_desc+1;
        (current_block_desc->page_desc->block_free)--;
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
	t_page_desc *page_desc;
	block_desc=(char *)address-sizeof(t_block_desc);
        page_desc=block_desc->page_desc;
	
	page_desc->block_free++;
	a_fixed_size_desc->current_free_block++;
	current_block_desc=address-sizeof(t_block_desc);
	first_block_desc=a_fixed_size_desc->first_block;
	a_fixed_size_desc->first_block=current_block_desc;
	current_block_desc->next_block=first_block_desc;
	current_block_desc->previous_block=NULL;
	if (first_block_desc!=NULL) first_block_desc->previous_block=current_block_desc;

	if (page_desc->block_free==a_fixed_size_desc->num_block
		&& (a_fixed_size_desc->current_free_block-page_desc->block_free)>a_fixed_size_desc->init_free_block)
        {
		current_block_desc=(char *) page_desc+sizeof(t_page_desc);
		for (index=0;index<a_fixed_size_desc->num_block;index++)
        	{	
			if (current_block_desc->next_block!=NULL && current_block_desc->previous_block!=NULL)
			{ 
				next_block_desc=current_block_desc->next_block;
				previous_block_desc=current_block_desc->previous_block;
				next_block_desc->previous_block=previous_block_desc;
				previous_block_desc->next_block=next_block_desc;
			}
			else if (current_block_desc->next_block==NULL && current_block_desc->previous_block!=NULL)
			{
				previous_block_desc=current_block_desc->previous_block;
				previous_block_desc->next_block=NULL;	
			}
			else if (current_block_desc->next_block!=NULL && current_block_desc->previous_block==NULL)
			{
				next_block_desc=current_block_desc->next_block;
				next_block_desc->previous_block=NULL;
				a_fixed_size_desc->first_block=next_block_desc;
			}
			else if (current_block_desc->next_block==NULL && current_block_desc->previous_block==NULL)
			{
				a_fixed_size_desc->first_block=NULL;	
			}
			current_block_desc=(char *)current_block_desc+sizeof(t_block_desc)+a_fixed_size_desc->block_size;
	       	}
		a_fixed_size_desc->current_free_block-=a_fixed_size_desc->num_block;
		a_buddy_free(page_desc);
	}
}

void a_fixed_size_dump(t_a_fixed_size_desc *a_fixed_size_desc) 
{
	int i=0;		
	t_block_desc *current_block_desc;
	current_block_desc=a_fixed_size_desc->first_block;
//	printf("*************************LIST DUMP*******************************\n");
//	printf("a_fixed_size_desc->has_initialized=%d \n",a_fixed_size_desc->has_initialized);
//      printf("a_fixed_size_desc->block_size=%d \n",a_fixed_size_desc->block_size);
//	printf("a_fixed_size_desc->num_block=%d \n",a_fixed_size_desc->num_block);
//	printf("a_fixed_size_desc->init_free_block=%d \n",a_fixed_size_desc->init_free_block);
//      printf("a_fixed_size_desc->current_free_block=%d \n",a_fixed_size_desc->current_free_block);
//	while (current_block_desc!=NULL)
//	{
//		printf("current_block_desc=%d \n",current_block_desc);
//		printf("current_block_desc->next_block=%d \n",current_block_desc->next_block);
//		printf("current_block_desc->previous_block=%d \n",current_block_desc->previous_block);
//		printf("current_block_desc->page_desc=%d \n",current_block_desc->page_desc);
//		printf("current_block_desc->page_desc->block_free=%d \n",current_block_desc->page_desc->block_free);
//		current_block_desc=current_block_desc->next_block;
//	}
//	printf("*****************************************************************\n");
	for (i=0;i<100;i++)
	{
		dump_fx[i]=((char*)(current_block_desc)+0xc);
		current_block_desc=current_block_desc->next_block;
	}
	return;
}










