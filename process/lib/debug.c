#include "lib.h"

void check_free_mem()
{
	unsigned int params[1];//??????????????????????????????

	SYSCALL(103,params);
}

//void check_free_pid(pid)
//{
//	unsigned int params[1];
//
//	SYSCALL(103,params);
//}



