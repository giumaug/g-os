#include "process_0.h"

void process_0() 
{
	unsigned int pid;
	
	pid=_fork();
	if (pid==0)
	{
		_exec("/shell");
	}
	else 
	{
		_exit(0);
	}
}
