#include "general.h"
#include "asm.h"
#include "lib/lib.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "process_0.h"

void process_0() 
{
	unsigned int pid;
	static char* argv[2];
	static char argv1[]="/shell";	
	
	argv[0]=argv1;
	argv[1]=NULL;	


	THREAD_FORK(pid);
	if (pid==0)
	{
//		THREAD_EXEC("/shell",argv);
		_exec("/shell",argv);
	}
	else 
	{
		THREAD_EXIT(0);
	}
}
