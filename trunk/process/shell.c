#include "lib/stdio.h" 
#include "lib/stdlib.h" 
#include "lib/string.h" 
#include "lib/unistd.h" 
#include "process/shell.h"

void shell() 
{
	unsigned int pid;	
	unsigned int i=0;
	unsigned int brk=0;
	unsigned int len;
	unsigned int is_background;
	char cmd[100];
	char* process_name;

	printf("g-shell v 0.1 \n");
	while(1)
	{
		printf("=>");
		scanf("%s",&cmd);
		printf("\n");
		if (strcmp(cmd,LIST_CMD)==0)
		{
			for (i=0;i<PROCESS_NUM;i++)
			{
				printf(user_process[i].name);
				printf("\n");
				
			}
		}
		else if (strncmp(cmd,RUN_CMD,3)==0)
		{
			i=0;
			brk=0;
			is_background=0;
			process_name=cmd+strlen(RUN_CMD)+1;
			while (!brk && i<PROCESS_NUM)
			{
				len=strlen(process_name);
				if (process_name[len-1]=='-') 
				{
					is_background=1;
					process_name[len-1]='\0';
				}
				if (strcmp(process_name,user_process[i].name)==0)
				{
					pid=fork();
					if (pid==0)
					{
						exec(user_process[i].start_addr,user_process[i].size);
					}
					else 
					{
						if (!is_background)
						{
							pause();
						}
						brk=1;
					}
				}
				i++;
			}
			if (brk==0)
				{
					printf("process not found \n");
				}
		}
		else 
		{
			printf("command not found \n");
		}
	}
}
