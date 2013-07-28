#ifndef STAT_HEADER
#ifndef ADD_HEADER
	#include "asm.h"
	#include "syscall_handler.h"
	#include "general.h"
	#define ADD_HEADER
#endif

extern inline int mkdir(const char *fullpath filename) __attribute__((always_inline)); 

extern inline int mkdir(const char *fullpath filename)
{
	unsigned int params[2];

	params[0]=fullpath;
	params[1]=flags;
	SYSCALL(23,params);
	return  params[1];
}

#endif
