#ifndef FIXED_SIZE_H                
#define FIXED_SIZE_H

#include "system.h"
#include "memory_manager/general.h"

typedef struct s_page_desc
{
	unsigned int block_free;
}
t_page_desc;

typedef struct s_block_desc
{
	struct s_block_desc *previous_block;
	struct s_block_desc *next_block;
}
t_block_desc;

typedef struct s_a_fixed_size_desc
{
	t_block_desc *first_block;
        unsigned int has_initialized;
        unsigned int block_size;
        unsigned int current_free_block;
}
t_a_fixed_size_desc;

void a_fixed_size_init(t_a_fixed_size_desc *a_fixed_size_desc,unsigned int block_size,void* mem_addr,int size);
void *a_fixed_size_alloc(t_a_fixed_size_desc *a_fixed_size_desc);
void a_fixed_size_free(t_a_fixed_size_desc *a_fixed_size_desc,void *address);
void a_fixed_size_check_mem_status();

#endif

