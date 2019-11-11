#include "lib.h"

int open(const char *fullpath, int flags)
{
	unsigned int params[3];

	params[0]=fullpath;
	params[1]=flags;
	printf("111111 \n");
	SYSCALL(18,params);
	return  params[2];
}

