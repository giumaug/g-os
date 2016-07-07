#include "lib/lib.h"  

int main (int argc, char* argv[])
{	
	int age;
	int pid;
	int proc_num;
	int i;

	char* reader="/flood";
	argv=malloc(sizeof(char*)*2);
	argv[0]=reader;
	age=0;

	while(1)
	{
		proc_num=(rand() % 5 + 1);
		//proc_num=50;
		printf("proc_num=%d \n",proc_num);
		for (i=0;i<proc_num;i++)
		{
			pid=fork();
			if (pid==0)
			{
				exec(argv[0],argv);
			}
		}
		age++;
		sleep(30000);
		check_free_mem();
	}
	exit(0);
}

