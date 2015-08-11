#include "lib/lib.h"  
#include "mem-stress.h"

int main (int argc, char* argv[])
{
	unsigned int n_request;
	unsigned int i;
	unsigned int age=0;
	unsigned int pid;
	char** params;
	char path[] = "/selection-sort";
	
	params=malloc(sizeof(char*)*2);
	params[0]=path;
	params[1]=NULL;

	check_free_mem();

//	while(1)
	{
//		n_request=(rand() % 100 + 1);
		n_request=2;
		printf("\nage=%d \n",age);
		printf("\nrequest number=%d \n",n_request);
		for (i=0;i<n_request;i++)
		{	
			pid=fork();
			if (pid==0)
			{	
				exec(params[0],params);
			}
			sleep(60000);
		}
//		age++;
//		sleep(60000);
//		printf("\n.........");
		check_free_mem();
	}
	exit(0);
}
