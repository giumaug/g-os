#include "asm.h"
#include "lib/lib.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "process_0.h"

void process_0() 
{
	unsigned int pid=5;
	
	THREAD_FORK(pid);
	if (pid==0)
	{
		THREAD_EXEC("/shell","/shell");
	}
	else 
	{
		THREAD_EXIT(0);
	}
}
