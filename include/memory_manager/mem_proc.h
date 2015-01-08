#ifndef MEM_PROC_H                
#define MEM_PROC_H

#include "system.h"

typedef struct s_mem_proc_desc
{
	void *mem_addr;
	unsigned int status;
}
t_mem_proc_desc;

void init_mem_proc();
void* alloc_mem_proc();
void free_mem_proc(void* addr);

#endif



