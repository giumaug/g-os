#ifndef KMALLOC_H                
#define KMALLOC_H

void init_kmoalloc();
void* kmalloc(unsigned int mem_size);
void kfree(void *address);
void  *_malloc(unsigned int mem_size);
void  _free(void *address);

#endif


