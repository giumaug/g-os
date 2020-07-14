#include "lib/lib.h"

int main(int argc, char* argv[])
{
	int i;
	int j;
	int file[100];
	char** file_names;
	char[] io_buffer ="aaaaaaaaaa";
	
	printf("-------process dev1 called-------\n");
	file_names = malloc(5 * 100);

	for (i = 0 ; i < 9; i++)
	{
		for (j = 0 ; j < 9; j++)
		{
			file_names[10 * i + j][0] = 'x';
			file_names[10 * i + j][1] = 'x';
			file_names[10 * i + j][2] = 48 + i;
			file_names[10 * i + j][3] = 48 + j;
	 		file_names[10 * i + j][4] = '\0';
		}	
	}

	for (i = 0; i < 100; i++)
	{
		f = open(file_names[i], O_CREAT | O_RDWR);
		file[i] = f;
	}

	for (i = 0; i < 100; i++)
	{
		write(file_names[i], io_buffer, 10);
	}
	
	











	remove(argv[1]);
	exit(0);
}
