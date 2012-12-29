#ifndef UNISTD_HEADER
#ifndef ADD_HEADER
	#include "asm.h"
	#include "syscall_handler.h"
	#include "general.h"
	#define ADD_HEADER
#endif

extern inline int pause(void) __attribute__((always_inline));
extern inline int fork() __attribute__((always_inline));
extern inline int exec(unsigned int start_addr,unsigned int size) __attribute__((always_inline));

extern inline int exec(unsigned int start_addr,unsigned int size) 
{
	unsigned int params[0];
	params[0]=start_addr;
	params[1]=size;
	SYSCALL(14,params);
	return 1;
}

extern inline int fork() 
{
	unsigned int params[1];
	unsigned int ret;
	params[0]=0;
	SYSCALL(1,params);
	ret=params[0];//ret val for parent
	params[0]=0;// ret val for child
	return ret;
}

extern inline int pause()
{
	SYSCALL(11,NULL);
}
#endif
