#ifndef BUDDY_H                
#define BUDDY_H

#include "system.h"
#include "memory_manager/general.h"

#define NUM_LIST 11
#define BLOCK_INDEX(mem_addr) (mem_addr-BUDDY_START_ADDR)/PAGE_SIZE

struct s_buddy_desc 
{ 	
	t_llist* page_list[11];
//	BUDDY_MEM_SIZE/PAGE_SIZE ACCOUNT FOR 0.0244 OF AVAILABLE MEMORY
	char order[BUDDY_MEM_SIZE/PAGE_SIZE];
	void* page_list_ref[BUDDY_MEM_SIZE/PAGE_SIZE];
	short count[BUDDY_MEM_SIZE/PAGE_SIZE];
	unsigned int free_mem_list[11];
	t_hashtable* page_desc;
}
typedef t_buddy_desc;

void buddy_init(t_buddy_desc* buddy);
void* buddy_alloc_page(t_buddy_desc* buddy,unsigned int mem_size);
void buddy_free_page(t_buddy_desc* buddy,void* page_addr);
void buddy_clean_mem(void* page_addr);
unsigned int buddy_free_mem(t_buddy_desc* buddy);
void buddy_check_mem_status();

#endif
