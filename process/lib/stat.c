#include "lib.h"

int mkdir(const char* fullpath)
{
	unsigned int params[1];

	params[0] = fullpath;
	SYSCALL(23, params);
	return  params[1];
}

int stat(char* pathname, t_stat* stat)
{
	void* params[3];

	params[0] = pathname;
	params[1] = stat;
	SYSCALL(27, params);
	return params[2];
}


