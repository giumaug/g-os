#ifndef KMALLOC_H                
#define KMALLOC_H

//#undef ASM_H
//#undef VM_H
//#undef FIXED_SIZE_H

#include "memory_manager/fixed_size.h"
#include "asm.h"
#include "system.h"
#include "memory_manager/general.h"
#include "virtual_memory/vm.h"
#include "lib/lib.h"
#include "debug.h"

void init_kmalloc();
void* kmalloc(unsigned int mem_size);
void kfree(void *address);
void* _malloc(unsigned int mem_size);
void  _free(void *address);
unsigned int kfree_mem();

#endif


