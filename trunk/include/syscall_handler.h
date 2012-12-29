#ifndef SYSCALL_HANDLER_H                
#define SYSCALL_HANDLER_H

int _fork(); 
void  *_malloc(unsigned int mem_size);
void  _free(void *address);
void syscall_handler();

#endif


