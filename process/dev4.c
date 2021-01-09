#include "lib/lib.h"

int main(int argc, char* argv[])
{
	int i, j, z, f, size;
	char io_buffer[] = "aaaaaaaaaa";
	char* file_name = NULL;

	size = (rand() % 21 + 1);
	file_name = malloc(25);

	for (i = 0; i < 10;i++)
	{
		for (j = 0 ; j < 10; j++)
		{
			size = (rand() % 21 + 1);
			file_name[z] = '/';
			for (z = 1; z < size; z++)
			{
				file_name[z] = 'x';
			}
			file_name[z] = 48 + i;
			file_name[z + 1] = 48 + j;
			file_name[z + 2] = '\0';
			printf("size is %d \n",size);
			printf("fileame is %s \n",file_name);
			//if (i == 9 && j == 6) 
			//{
			//	exit(0);
			//}
			f = open(file_name, O_CREAT | O_RDWR); 
			write(f,io_buffer,10);
			close(f);
		}
	}
	free(file_name);
	exit(0);
}
