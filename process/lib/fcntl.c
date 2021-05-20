#include "lib.h"

int open(const char *fullpath, int flags)
{
	unsigned int params[3];

	params[0]=fullpath;
	params[1]=flags;
	SYSCALL(18,params);
	return  params[2];
}

void flush_inode_cache()
{
	unsigned int params[1];

	SYSCALL(200, params);
}

