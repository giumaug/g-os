#include "lib/lib.h"  
#include "shell.h"

int main() 
{
	unsigned int pid;	
	unsigned int is_background;
	unsigned int len;
	char cmd[100];

	printf("g-shell v 0.1 \n");
	while(1)
	{
		printf("=>");
		scanf("%s",&cmd);
		printf("\n");

		len=strlen(cmd);
		if (cmd[len-1]=='-') 
		{
			is_background=1;
			cmd[len-1]='\0';
		}
		pid=fork();
		if (pid==0)
		{
			exec(cmd);
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

//	while(1)
//	{
//		printf("=>");
//		scanf("%s",&cmd);
//		printf("\n");
//		if (strcmp(cmd,LIST_CMD)==0)
//		{
//			for (i=0;i<PROCESS_NUM;i++)
//			{
//				printf(user_process[i].name);
//				printf("\n");
//				
//			}
//		}
//		else if (strncmp(cmd,RUN_CMD,3)==0)
//		{
//			i=0;
//			brk=0;
//			is_background=0;
//			process_name=cmd+strlen(RUN_CMD)+1;
//			while (!brk && i<PROCESS_NUM)
//			{
//				len=strlen(process_name);
//				if (process_name[len-1]=='-') 
//				{
//					is_background=1;
//					process_name[len-1]='\0';
//				}
//					pid=fork();
//					if (pid==0)
//					{
//						exec(process_name);
//					}
//					else 
//					{
//						if (!is_background)
//						{
//							pause();
//						}
//						brk=1;
//					}
//				i++;
//			}
//			if (brk==0)
//				{
//					printf("process not found \n");
//				}
//		}
//		else 
//		{
//			printf("command not found \n");
//		}
//	}
}
