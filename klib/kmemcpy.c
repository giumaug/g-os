#include "klib/kmemcpy.h"

void *kmemcpy(void *dest, const void *src, unsigned int size)
{
	char *dest_addr = dest;
	while (size-- > 0)
	{
    		*(char*)dest++ = *(char*)src++;
	}
  	return dest_addr;
}

void kfillmem(void *dest, const int val, unsigned int size)
{
	char *dest_addr = dest;
	while (size-- > 0)
	{
    		*(char*)dest++ = val;
	}
  	return dest_addr;
}



