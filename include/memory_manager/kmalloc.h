#ifndef KMALLOC_H                
#define KMALLOC_H

#define NUM_POOL 10
#define MEM_AVAILABLE 104857600
#define MEM_TO_POOL MEM_AVAILABLE/NUM_POOL
//#define HEADER_MEM(block_size) HEADER_MEM_PAGE(block_size)*(MEM_TO_POOL/PAGE_SIZE) 
//#define INIT_FREE_BLOCK(block_size) (MEM_TO_POOL-HEADER_MEM(block_size))/block_size
#define INIT_FREE_BLOCK(block_size) (MEM_TO_POOL/PAGE_SIZE)*((PAGE_SIZE-SIZE_OF_PAGE_DESC)/(block_size+SIZE_OF_BLOCK_DESC))

void init_kmoalloc();
void* kmalloc(unsigned int mem_size);
void kfree(void *address);
void  *_malloc(unsigned int mem_size);
void  _free(void *address);

#endif


