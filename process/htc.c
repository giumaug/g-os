#include "lib/lib.h"  
#include "htc.h"

int main (int argc, char* argv[])
{
	int pid;
	char** params_1;
	char** params_2;
	char** params_3;
	char** params_4;
	char** params_5;
	
	char p_name[] = "/htcs";
	char path_1[] = "/tcp.c";
	char path_2[] = "/scheduler.c";
	char path_3[] = "/vm.c";
	char path_4[] = "/8042.c";
	char path_5[] = "/ext2.c";
	
	params_1 = malloc(sizeof(char*) * 4);
	params_1[0] = p_name;
	params_1[1] = path_1;
	params_1[2] = path_1;
	params_1[3] = NULL;

	params_2 = malloc(sizeof(char*) * 4);
	params_2[0] = p_name;
	params_2[1] = path_2;
	params_2[2] = path_2;
	params_2[3] = NULL;

	params_3 = malloc(sizeof(char*) * 4);
	params_3[0] = p_name;
	params_3[1] = path_3;
	params_3[2] = path_3;
	params_3[3] = NULL;

	params_4 = malloc(sizeof(char*) * 3);
	params_4[0] = p_name;
	params_4[1] = path_4;
	params_4[2] = path_4;
	params_4[3] = NULL;

	params_5 = malloc(sizeof(char*) * 4);
	params_5[0] = p_name;
	params_5[1] = path_5;
	params_5[2] = path_5;
	params_5[3] = NULL;

	pid = fork();
	if (pid == 0)
	{		
		exec(params_1[0], params_1);
		printf("after exec \n");
	}

	pid = fork();
	if (pid == 0)
	{	
		exec(params_2[0],params_2);
		printf("after exec \n");
	}

	pid = fork();
	if (pid == 0)
	{	
		exec(params_3[0], params_3);
		printf("after exec \n");
	}

	pid = fork();
	if (pid == 0)
	{	
		exec(params_4[0],params_4);
		printf("after exec \n");
	}

	pid = fork();
	if (pid == 0)
	{	
		exec(params_5[0], params_5);
		printf("after exec \n");
	}
	exit(0);
}
			 
