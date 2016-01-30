#include "lib/lib.h"  
#include "mem-stress.h"

int main (int argc, char* argv[])
{
	int pid;
	char** params;
	char path[] = "/selection-sort";
	
	params=malloc(sizeof(char*)*2);
	params[0]=path;
	params[1]=NULL;

	pid=fork();
	if (pid==0)
	{	
		exec(params[0],params);
	}
	sleep(1000);
	exit(0);
}
