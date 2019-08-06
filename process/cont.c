#include "lib/lib.h"  
#include "shell.h"

static void cd(char* path);

int main (int _argc, char* _argv[])
{
	char** argv;
	int pid,i,ret,child_pid;
	char* reader="/tcpf";
	argv=malloc(sizeof(char*)*2);
	argv[0]=reader;
	argv[1]=0;

	for (i = 0;i < 1000;i++)
	{
		printf("age_is %d \n",i);
		pid = fork();
		if (pid == 0)
		{
			child_pid = getpid();
			setpgid(child_pid,child_pid);
			tcsetpgrp(child_pid);
			ret = exec(argv[0],argv);
			if (ret == -1)
			{
				printf("error.");
				exit(0);
			}	
		}
		else
		{
			sleep(5000);
			signal();
		}
	}
	exit(0);
}
