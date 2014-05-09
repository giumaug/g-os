#include "lib/stdio.h"
#include "lib/unistd.h"
#include "lib/fcntl.h"
#include "lib/stdlib.h"

/*
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
//		n_request=25;
		n_request=10;
		printf("\nage=");
		d_printf(age);
		printf("\nrequest number.....=");
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
		printf("\n+++++++++");
		sleep(60000);
		printf("\n.........");
		check_free_mem();
	}
	exit(0);
}
*/

process_1()
{
	int i,f;
        char* io_buffer;
	
	io_buffer=malloc(20);
	f=open("/tmp/read_test.txt", O_RDWR);
	read(f,io_buffer,20);
	close(f);

	for(i=0;i<20;i++)
	{
		printf(io_buffer[i]);
	}
	free(io_buffer);
}



