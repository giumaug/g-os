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
	printf("argc=%d \n",&_argc);
	index=_argv[0];
	printf("val=%d \n",&index);
	printf("argv[0]=%s \n",_argv[0]);

	while(1)
	{
		argc=0;
		printf("=>");
		scanf("%s",&cmd);
		//printf(cmd);

		if (cmd[0]!='/' && !(cmd[0]=='.' && cmd[1]=='/'))
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
		
			//BUILT IN COMMAND!!!		
			if (cmd[index]=='c' && cmd[index+1]=='d' && cmd[index+2]==' ')
			{
				cd(&cmd[index+3]);
				printf("\n");
			}
			else
			{
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
					printf("\n");
					j=0;
					while (cmd[i]!=' ' && cmd[i]!=NULL)
					{		
						j++;
						i++;
					}
					i++;
					argv[k]=malloc(j+1);
//					printf("\n");
//					printf("k=");
//					print_num(k);
//					printf("\n");
//					printf("j=");
//					print_num(j+1);
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
//					printf("\n");
//					printf("k=");
//					print_num(k);
//					printf("\n");
//					printf("j=");
//					print_num(j);
			
				}
				
//				pid=5;
				pid=fork();

//				static unsigned int* kkk;
//				kkk=0xbfffbf2c;
//				*kkk=1;

				if (pid==0)
				{
					//ret=execv(argv[0],argv);
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



