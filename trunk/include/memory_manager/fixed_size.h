#ifndef FIXED_SIZE_H                
#define FIXED_SIZE_H

#include "memory_manager/general.h"

#define a_buddy_free(PAGE_ADDRESS) kfree(PAGE_ADDRESS)
#define NUM_BLOCK(block_size) ((PAGE_SIZE-sizeof(t_page_desc))/(sizeof(t_block_desc)+block_size)) 
#define NUM_PAGE(init_free_block,block_size) ((init_free_block%NUM_BLOCK(block_size))==0 ? (init_free_block/NUM_BLOCK(block_size)) : (init_free_block/NUM_BLOCK(block_size)+1))
#define SIZE_OF_BLOCK_DESC sizeof(t_block_desc)
#define SIZE_OF_PAGE_DESC sizeof(t_page_desc)

typedef struct s_page_desc
{
	unsigned int block_free;
}
t_page_desc;

typedef struct s_block_desc
{
	struct s_block_desc *previous_block;
	struct s_block_desc *next_block;
	t_page_desc *page_desc;
}
t_block_desc;

typedef struct s_a_fixed_size_desc
{
	t_block_desc *first_block;
        unsigned int has_initialized;
        unsigned int block_size;
	unsigned int num_block;
	unsigned int init_free_block;
        unsigned int current_free_block;
	unsigned int num_page;
}
t_a_fixed_size_desc;

void a_fixed_size_init(t_a_fixed_size_desc *a_fixed_size_desc,unsigned int block_size,unsigned int init_free_block);
void *a_fixed_size_alloc(t_a_fixed_size_desc *a_fixed_size_desc);
void a_fixed_size_free(t_a_fixed_size_desc *a_fixed_size_desc,void *address);

#endif

