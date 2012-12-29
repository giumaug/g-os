#include "lib/unistd.h"
#include "lib/stdlib.h"
#include "process/process_0.h"

void process_0() 
{
	unsigned int pid;
	pid=fork();
	if (pid==0)
	{
		exec(SHELL_START_ADDR,SHELL_SIZE);
	}
	else 
	{
		exit(0);
	}
}
