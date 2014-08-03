#include "lib/lib.h"  
#include "shell.h"

int main (int _argc, char *_argv[])
{
	unsigned int pid;	
	unsigned int is_background;
	unsigned int len,i,k,j;
	unsigned int argc=0;
	char cmd[100];
	char** argv;
	char c;

	printf("g-shell v 0.1 \n");
	printf("argc=");
	print_num(_argc);
	printf("\n");
	//printf("argv[0]=");
	//printf(_argv[0]);
	printf("\n");

	while(1)
	{
		printf("=>");

		i=0;
		do
		{
			c=getc();
			cmd[i++]=c;
		}
		while (c!='\n');
		cmd[i-1]=NULL;

		printf("\n");
		len=strlen(cmd);
		if (cmd[len-1]=='-') 
		{
			is_background=1;
			cmd[len-1]='\0';
		}

		i=0;
		while (cmd[i]!=NULL)
		{
			if(cmd[i]==' ')
			{
				argc++;
			}
			i++;
		}

		argv=malloc(sizeof(char*)*argc+1);
		i=0;
		for(k=0;k<=argc;k++)
		{
			j=0;
			while (cmd[i]!=' ' && cmd[i]!=NULL)
			{		
				j++;
				i++;
			}
			argv[k]=malloc(j+1);
		}

		i=0;
		j=0;
		for(k=0;k<=argc;k++)
		{
			j=0;
			while (cmd[i]!=' ' && cmd[i]!=NULL)
			{		
				argv[k][j++]=cmd[i];
				i++;

			}
			i++;
			argv[k][j++]='\0';
		}

		pid=fork();
		if (pid==0)
		{
			//exec(argv[0],argv);
			exec(argv[0],argv);
		}
		else 
		{
			if (!is_background)
			{
				//pause();
			}
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
