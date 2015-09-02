#include "lib.h"

int mkdir(const char *fullpath)
{
	unsigned int params[1];

	params[0]=fullpath;
	SYSCALL(23,params);
	return  params[1];
}

int stat(const char *pathname, struct stat *buf)
{

}


