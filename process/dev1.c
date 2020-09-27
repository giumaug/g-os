#include "lib/lib.h"

int main(int argc, char* argv[])
{
	int i;
	int j;
	int file[100];
	int f;
	char** file_names;
	char buffer[] = "aaaaaaaaaa";
	char path_1[] = "/aaa";
	char path_2[] = "/aaa/ddd1";
	char path_3[] = "/aaa/ddd1/ddd2";
	
	mkdir(path_1);
	mkdir(path_2);
	mkdir(path_3);

	write_file(path_1);
	write_file(path_2);
	write_file(path_3);

	//remove(argv[1]);
	remove("/aaa");
	exit(0);
}

void write_file(char* src_path)
{
	int i;
	int j;
	int file[100];
	int f;
	int src_path_len;
	char** file_names;
	char buffer[] = "aaaaaaaaaa";

	src_path_len = strlen(src_path);
	file_names = malloc(100 * sizeof(char*));
	for ( i = 0 ; i < 100; i++)
	{
		file_names[i] = malloc(30 + src_path_len);
		strcpy(file_names[i], src_path);
	}

	for (i = 0 ; i <= 9; i++)
	{
		for (j = 0 ; j <= 9; j++)
		{
			file_names[10 * i + j][src_path_len] = '/';
			file_names[10 * i + j][src_path_len + 1] = 'x';
			file_names[10 * i + j][src_path_len + 2] = 'x';

			file_names[10 * i + j][src_path_len + 3] = 'x';
			file_names[10 * i + j][src_path_len + 4] = 'x';
			file_names[10 * i + j][src_path_len + 5] = 'x';
			file_names[10 * i + j][src_path_len + 6] = 'x';
			file_names[10 * i + j][src_path_len + 7] = 'x';
			file_names[10 * i + j][src_path_len + 8] = 'x';
			file_names[10 * i + j][src_path_len + 9] = 'x';

			file_names[10 * i + j][src_path_len + 10] = 'x';
			file_names[10 * i + j][src_path_len + 11] = 'x';
			file_names[10 * i + j][src_path_len + 12] = 'x';
			file_names[10 * i + j][src_path_len + 13] = 'x';
			file_names[10 * i + j][src_path_len + 14] = 'x';
			file_names[10 * i + j][src_path_len + 15] = 'x';
			file_names[10 * i + j][src_path_len + 16] = 'x';
			file_names[10 * i + j][src_path_len + 17] = 'x';
			file_names[10 * i + j][src_path_len + 18] = 'x';
			file_names[10 * i + j][src_path_len + 19] = 'x';
			file_names[10 * i + j][src_path_len + 20] = 'x';

			file_names[10 * i + j][src_path_len + 21] = 48 + i;
			file_names[10 * i + j][src_path_len + 22] = 48 + j;
	 		file_names[10 * i + j][src_path_len + 23] = '\0';
		}
	}

//	for (i = 0; i <= 99; i++)
//	{
//		printf("name is %s \n",file_names[i]);
//	}

	for (i = 0; i <= 99; i++)
	{
		f = open(file_names[i], O_CREAT | O_RDWR);
		file[i] = f;
	}

	for (i = 0; i <= 99; i++)
	{
		write(file[i], buffer, 10);
	}
	
	for (i = 0; i <= 99; i++)
	{
		close(file[i]);
	}
}
