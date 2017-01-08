#ifndef LIB_H                
#define LIB_H

#include "system.h"

void printk(char *text,...);
void kmemcpy(void *dest, const void *src, unsigned int size);
void kfillmem(void *dest, const int val, unsigned int size);
unsigned int pow2(unsigned int x);
unsigned int log2(unsigned int x);
u32 min(u32 x,u32 y);
u32 max(u32 x,u32 y);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2,unsigned int n);
unsigned int strlen(const char* s);

#endif

