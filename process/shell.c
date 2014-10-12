#include "lib/lib.h"  
#include "shell.h"

int main (int _argc, char *_argv[])
{
	int ret;
	unsigned int pid;	
	unsigned int is_background;
	unsigned int len,i,k,j;
	unsigned int argc=0;
	char cmd[100];
	char** argv;
	char c;
	unsigned int pippo;

	printf("g-shell v 0.1 \n");
	printf("argc=");
	print_num(_argc);
	printf("\n");
	printf("argv[0]=");
	printf(_argv[0]);
	printf("\n");

	while(1)
	{
		argc=0;
		printf("=>");

		scanf("%s",&cmd);
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
			printf("argc=");
			print_num(argc);
			printf("\n");
			ret=exec(argv[0],argv);
			if (ret==-1)
			{
				printf("\n");
				printf("Command not found.");
				printf("\n");
				exit(0);
			}
		}
		else 
		{
			if (!is_background)
			{
				pause();
			}
		}
	}
}
