#include "process_0.h"

void process_0();

void process_0() 
{
	unsigned int pid;
	
	pid=fork();
	if (pid==0)
	{
		exec("shell");
	}
	else 
	{
		exit(0);
	}
}
