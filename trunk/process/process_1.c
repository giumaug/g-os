#include "lib/unistd.h"
#include "lib/stdio.h" 
#include "lib/stdlib.h" 

#define SEED 105491

process_1()
{
	unsigned int seed=SEED;
	unsigned int n_request;
	unsigned int i;
	unsigned int age=0;
	unsigned int pid;
	
//	while(1)
//	{
		n_request=(rand(&seed) % 100 + 1);
		n_request=1;
		printf("\nage=");
		d_printf(age);
		printf("\nrequest number=");
		d_printf(n_request);
		for (i=0;i<n_request;i++)
		{	
			pid=fork();
			if (pid==0)
			{	
				exec(0x700000,0x100000);
			}
		}
		age++;
		//sleep(20000);
		//check_free_mem();
//	}
	exit(0);
}
