#ifndef BUDDY_H                
#define BUDDY_H

#include "memory_manager/general.h"

//Consider system with 200M.Buddy takes 185M starting from 15M 
//0xD00000
#define MEM_START_ADDR 0x1700000
#define MEM_END_ADDR PHY_MEM_SIZE
#define ALLOCATED_MEM MEM_END_ADDR - MEM_START_ADDR
#define MEM_PAGE (ALLOCATED_MEM)/PAGE_SIZE
#define MEM_INDEX(address) (address-MEM_START_ADDR)/PAGE_SIZE

typedef struct s_page_frame_desc
{
	void *page_add;
	int owner_id;
}
t_page_frame_desc;

typedef struct s_mem_desc
{
	t_page_frame_desc page_desc[MEM_PAGE];
	unsigned int index;	
}
t_mem_desc;

void buddy_init_mem();
void *buddy_alloc_mem(unsigned int owner_id);
void buddy_free_mem();

#endif
