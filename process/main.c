#include "lib/lib.h"  

int main (int argc, char* argv[])
{	
	int age;
	int pid;

	char* reader="/flood";
	argv=malloc(sizeof(char*)*2);
	argv[0]=reader;
	age=0;

	while(1)
	{
		pid=fork();
		if (pid==0)
		{
			exec(argv[0],argv);
		}
		age++;
		sleep(30000);
		check_free_mem();
	}
	exit(0);
}

