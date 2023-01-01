#ifndef KMALLOC_H                
#define KMALLOC_H

//#undef ASM_H
//#undef VM_H
//#undef FIXED_SIZE_H

#define ALGND_ADDR_MAP_INIT_SIZE 100

#include "memory_manager/fixed_size.h"
#include "asm.h"
#include "system.h"
#include "memory_manager/general.h"
#include "virtual_memory/vm.h"
#include "lib/lib.h"
#include "debug.h"

void init_kmallocs();
void* kmalloc(unsigned int mem_size);
void kfree(void *address);
void* _malloc(unsigned int mem_size);
void  _free(void *address);
unsigned int kfree_mem();
void* aligned_kmalloc(u32 mem_size, u32 alignment);
void aligned_kfree(void* address);

#endif


