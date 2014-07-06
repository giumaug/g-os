#include "lib/lib.h"
#include "process_0.h"

void process_0() 
{
	unsigned int pid;
	
	THREAD_FORK(pid);
	if (pid==0)
	{
		THREAD_EXEC("/shell");
	}
	else 
	{
		THREAD_EXIT(0);
	}
}
