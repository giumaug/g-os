#include "lib/lib.h"  
#include "dev3.h"

int main (int argc, char* argv[])
{
	int pid;
	char** params_1;
	char** params_2;
	char** params_3;
	char** params_4;
	char** params_5;
	char** params_6;
	char** params_7;
	char** params_8;
	char** params_9;
	char** params_10;
	char** params_11;
	char** params_12;
	char** params_13;
	char** params_14;
	char** params_15;
	char** params_16;
	char** params_17;
	char** params_18;
	char** params_19;
	char** params_20;
	char p_name[] = "/dev2";
	char path_1[] = "/xxx1.txt";
	char path_2[] = "/xxx2.txt";
	char path_3[] = "/xxx3.txt";
	char path_4[] = "/xxx4.txt";
	char path_5[] = "/xxx5.txt";
	char path_6[] = "/xxx6.txt";
	char path_7[] = "/xxx7.txt";
	char path_8[] = "/xxx8.txt";
	char path_9[] = "/xxx9.txt";
	char path_10[] = "/xxx10.txt";
	char path_11[] = "/xxx11.txt";
	char path_12[] = "/xxx12.txt";
	char path_13[] = "/xxx13.txt";
	char path_14[] = "/xxx14.txt";
	char path_15[] = "/xxx15.txt";
	char path_16[] = "/xxx16.txt";
	char path_17[] = "/xxx17.txt";
	char path_18[] = "/xxx18.txt";
	char path_19[] = "/xxx19.txt";
	char path_20[] = "/xxx20.txt";
	
	params_1 = malloc(sizeof(char*) * 3);
	params_1[0] = p_name;
	params_1[1] = path_1;
	params_1[2] = NULL;

	params_2 = malloc(sizeof(char*) * 3);
	params_2[0] = p_name;
	params_2[1] = path_2;
	params_2[2] = NULL;

	params_3 = malloc(sizeof(char*) * 3);
	params_3[0] = p_name;
	params_3[1] = path_3;
	params_3[2] = NULL;

	params_4 = malloc(sizeof(char*) * 3);
	params_4[0] = p_name;
	params_4[1] = path_4;
	params_4[2] = NULL;

	params_5 = malloc(sizeof(char*) * 3);
	params_5[0] = p_name;
	params_5[1] = path_5;
	params_5[2] = NULL;

	params_6 = malloc(sizeof(char*) * 3);
	params_6[0] = p_name;
	params_6[1] = path_6;
	params_6[2] = NULL;

	params_7 = malloc(sizeof(char*) * 3);
	params_7[0] = p_name;
	params_7[1] = path_7;
	params_7[2] = NULL;

	params_8 = malloc(sizeof(char*) * 3);
	params_8[0] = p_name;
	params_8[1] = path_8;
	params_8[2] = NULL;

	params_9 = malloc(sizeof(char*) * 3);
	params_9[0] = p_name;
	params_9[1] = path_9;
	params_9[2] = NULL;

	params_10 = malloc(sizeof(char*) * 3);
	params_10[0] = p_name;
	params_10[1] = path_10;
	params_10[2] = NULL;

	params_11 = malloc(sizeof(char*) * 3);
	params_11[0] = p_name;
	params_11[1] = path_11;
	params_11[2] = NULL;

	params_12 = malloc(sizeof(char*) * 3);
	params_12[0] = p_name;
	params_12[1] = path_12;
	params_12[2] = NULL;

	params_13 = malloc(sizeof(char*) * 3);
	params_13[0] = p_name;
	params_13[1] = path_13;
	params_13[2] = NULL;

	params_14 = malloc(sizeof(char*) * 3);
	params_14[0] = p_name;
	params_14[1] = path_14;
	params_14[2] = NULL;

	params_15 = malloc(sizeof(char*) * 3);
	params_15[0] = p_name;
	params_15[1] = path_15;
	params_15[2] = NULL;

	params_16 = malloc(sizeof(char*) * 3);
	params_16[0] = p_name;
	params_16[1] = path_16;
	params_16[2] = NULL;

	params_17 = malloc(sizeof(char*) * 3);
	params_17[0] = p_name;
	params_17[1] = path_17;
	params_17[2] = NULL;

	params_18 = malloc(sizeof(char*) * 3);
	params_18[0] = p_name;
	params_18[1] = path_18;
	params_18[2] = NULL;

	params_19 = malloc(sizeof(char*) * 3);
	params_19[0] = p_name;
	params_19[1] = path_19;
	params_19[2] = NULL;

	params_20 = malloc(sizeof(char*) * 3);
	params_20[0] = p_name;
	params_20[1] = path_20;
	params_20[2] = NULL;

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
	pid = fork();
	if (pid == 0)
	{	
		exec(params_6[0],params_6);
		printf("after exec \n");
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_7[0], params_7);
		printf("after exec \n");
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_8[0],params_8);
		printf("after exec \n");
	}
/*
	pid = fork();
	if (pid == 0)
	{	
		exec(params_9[0], params_9);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_10[0],params_10);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_11[0], params_11);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_12[0],params_12);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_13[0], params_13);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_14[0],params_14);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_15[0], params_15);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_16[0],params_16);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_17[0], params_17);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_18[0],params_18);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_19[0], params_19);
	}
	pid = fork();
	if (pid == 0)
	{	
		exec(params_20[0],params_20);
	}
*/
	exit(0);
}
			 
