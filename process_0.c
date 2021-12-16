#include "asm.h"
#include "lib/lib.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "process_0.h"

//_exec SWITCHES KERNEL THREAD TO USERSPACE PROCESS
// THREAD_FORK CLONES KERNEL THREAD
// THREAD_EXEC CREATES NEW KERNEL THREAD TO BE IMPLEMENTED!!!!!!!!!
void process_0() 
{
	unsigned int pid;
	unsigned int child_pid;
	static char* argv[2];
	static char argv1[] = "/shell";	
	
	argv[0] = argv1;
	argv[1] = NULL;	

    THREAD_FORK(pid);
	if (pid == 0)
	{
		child_pid = _getpid();
		_setpgid(child_pid,child_pid);
		_tcsetpgrp(child_pid);
		_exec("/shell",argv);	
	}
	else 
	{
		THREAD_EXIT(0);
	}
}
