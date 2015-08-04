#include "lib.h"

void check_free_mem()
{
	unsigned int params[0];

	printf("-------------------inside \n");
	SYSCALL(103,params);
}

