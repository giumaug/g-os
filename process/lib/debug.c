#include "lib.h"

void check_free_mem()
{
	unsigned int params[0];

	SYSCALL(103,params);
}

