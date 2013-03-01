#include "lib/unistd.h"
#include "lib/stdlib.h"
#include "process/process_1.h"

void process_1()
{
	int i;
	int pid;

	pid=fork();
	if (pid==0)
	{	
		exec(0x700000,0x100000);
	}

	pid=fork();
	if (pid==0)
	{	
		exec(0x800000,0x100000);
	}

	pid=fork();
	if (pid==0)
	{	
		exec(0xa00000,0x100000);
	}
	exit(0);
}


