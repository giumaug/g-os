#include "system.h"

void printk(char *text,...);
void kmemcpy(void *dest, const void *src, unsigned int size);
void kfillmem(void *dest, const int val, unsigned int size);
unsigned int pow2(unsigned int x);
unsigned int log2(unsigned int x);

int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2,unsigned int n);
unsigned int strlen(const char* s);

