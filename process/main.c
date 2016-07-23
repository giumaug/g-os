#include "lib/lib.h"  

int main (int argc, char* argv[])
{	
	int age;
	int pid;
	int proc_num;
	int file_id;
	int i;
	char param[3];

	char* reader="/srcflood";
	argv=malloc(sizeof(char*)*3);
	argv[0]=reader;
	age=0;

	while(1)
	{
		proc_num=(rand() % 5 + 1);
		for (i=0;i<proc_num;i++)
		{
			file_id=(rand() % 83 + 1);
			printf("file id is %d \n",file_id);
			file_id=file_id+255;
			param[0]=((unsigned char*) &(file_id))[1];
			param[1]=((unsigned char*) &(file_id))[0];
			param[2]=0;
			argv[1]=param;
			argv[2]=0;
			pid=fork();
			if (pid==0)
			{
				exec(argv[0],argv);
			}
		}
		age++;
		sleep(60000);
		check_free_mem();
	}
	exit(0);
}

