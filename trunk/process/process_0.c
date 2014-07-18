#include "lib/lib.h"
#include "process_0.h"

void process_0() 
{
	unsigned int pid;
	char* args[2];

	args[0]="/shell";
	args[1]=NULL;	
	
	THREAD_FORK(pid);
	if (pid==0)
	{
		THREAD_EXEC("/shell",args);
	}
	else 
	{
		THREAD_EXIT(0);
	}
}
