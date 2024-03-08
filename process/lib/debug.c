#include "lib.h"

void panic()
{
	unsigned int params[1];

	SYSCALL(104,params);
}

void check_free_mem()
{
	unsigned int params[1];

	SYSCALL(103,params);
}

void check_free_pid(pid)
{
	unsigned int params[1];

	SYSCALL(103,params);
}

void check_open_conn()
{
	unsigned int params[1];

	SYSCALL(105,params);
}



