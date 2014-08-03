#include "lib/lib.h"
#include "process_0.h"

void process_0() 
{
	unsigned int pid;
	char* args[2];

	args[0]="/shell";
	args[1]=NULL;	
	
//	--HREAD_FORK(pid);
	if (pid==0)
	{
		//HREAD_EXEC("/shell",args);
	}
	else 
	{
		//HREAD_EXIT(0);
	}
}
