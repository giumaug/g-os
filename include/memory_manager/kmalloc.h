#ifndef KMALLOC_H                
#define KMALLOC_H

#include "system.h"

void init_kmalloc();
void* kmalloc(unsigned int mem_size);
void kfree(void *address);
void* _malloc(unsigned int mem_size);
void  _free(void *address);
unsigned int kfree_mem();

#endif


