#include "lib/lib.h"  
#include "shell.h"

static void cd(char* path);

int main (int _argc, char* _argv[])
{
	int ret;
	unsigned int pid;	
	unsigned int is_background;
	unsigned int len,i,k,j;
	unsigned int argc=0;
	char cmd[100];
	char** argv;
	char c;
	unsigned int index;
	
	printf("g-shell v 0.1 \n");
	printf("argc=%d \n",_argc);
	printf("argv[0]=%s \n",_argv[0]);

	while(1)
	{
		argc=0;
		printf("=>");
		scanf("%s",&cmd);

		//BUILT IN COMMAND!!!
		if (cmd[0]=='c' && cmd[1]=='d' && cmd[2]==' ')
		{
				cd(&cmd[index+3]);
				printf("\n");
		}

		else if (cmd[0]!='/' && !(cmd[0]=='.' && cmd[1]=='/'))
		{
			printf("\n");
			printf("Command not found.");
			printf("\n");
		}
		else
		{
			if (cmd[0]=='/')
			{
				index=1;
			}
			else if (cmd[0]=='.' && cmd[1]=='/')
			{
				index=2;
			}
			
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
		
			argv=malloc(sizeof(char*)*(argc+2));
				
			i=0;
			for(k=0;k<=argc;k++)
			{
				printf("\n");
				j=0;
				while (cmd[i]!=' ' && cmd[i]!=NULL)
				{		
					j++;
					i++;
				}
				i++;
				argv[k]=malloc(j+1);
			}

			i=0;
			j=0;
			for(k=0;k<=argc;k++)
			{
				j=0;
				while (cmd[i]!=' ' && cmd[i]!=NULL)
				{	
					argv[k][j]=cmd[i];
					j++;
					i++;
				}
				i++;
				argv[k][j++]='\0';
				printf("argv=%s \n",argv[k]);
			
			}
			argv[argc+1]=NULL;
			pid=fork();

			if (pid==0)
			{
				//ret=execv(argv[0],argv);
				ret=exec(argv[0],argv);

				for(k=0;k<=argc;k++)
				{
					free(argv[k]);
				}
				free(argv);

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
					printf("woken up \n");
				}
			}
		}
	}
}

static void cd(char* path)
{
	int ret;

	ret=chdir(path);
	if (ret==-1)
	{
		printf("\n");
		printf("wrong arguments");
		printf("\n");
	}
}



