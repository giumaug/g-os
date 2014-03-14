#include "lib/unistd.h"
#include "lib/stdio.h" 
#include "lib/stdlib.h" 

process_1()
{
	unsigned int n_request;
	unsigned int i;
	unsigned int age=0;
	unsigned int pid;
	unsigned int params[1];
	unsigned int count;
	unsigned int ret;
	
	while(1)
	{
		n_request=(rand() % 100 + 1);
		n_request=25;
//		n_request=10;
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
		sleep(20000);
		count=625;
/*		
		while (count>0)
		{
			SYSCALL(102,params);
			ret=params[0];
			if (ret==1) 
			{
				count--;
			}
			sleep(1000);
		}
*/
		check_free_mem();
	}
	exit(0);
}
