#include "lib/lib.h"

void kmemcpy(void *dest, const void *src, unsigned int size)
{
	char *dest_addr = dest;
	while (size-- > 0)
	{
    		*(char*)dest++ = *(char*)src++;
	}
  	return dest_addr;
}

void kmemcpy_2(void *dest, const void *src, unsigned int size)
{
	#ifdef PROFILE
	long long time_1;
	long long time_2;
	rdtscl(&time_1);
	#endif

	char *dest_addr = dest;
	while (size-- > 0)
	{
    		*(char*)dest++ = *(char*)src++;
	}
	#ifdef PROFILE
	rdtscl(&time_2);
	system.time_counter_12++;
	system.timeboard_12[system.time_counter_12] = (time_2 - time_1);
	#endif
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



