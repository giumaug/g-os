#include "lib/lib.h"

int main(int argc, char* argv[])
{
	int f;
	int i;
	int b_read;
	char* io_buffer_a = NULL;
	char* io_buffer_b = NULL;
/*
	select_dev(1);
	io_buffer = malloc(4100);
	f = open("/zero.txt", O_RDWR | O_APPEND);
	if (f == -1)
	{
		printf("file not found\n");
		return;
	}
	for (i = 0; i < 1; i++)
	{
		b_read = read(f, io_buffer, 4096);
	}
	io_buffer[b_read] ='\0';
	close(f);
	printf("read string %s \n",io_buffer);	
	free(io_buffer);
*/

        io_buffer_a = malloc(11);
	io_buffer_b = malloc(11);

	//printf("argv[0] is- %s \n",argv[0]);
	//printf("argv[1] is- %s \n",argv[1]);
	for (i = 0; i < 9; i++)
	{
		io_buffer_a[i] = 'a';
		io_buffer_b[i] = 'b';
	}
	io_buffer_a[9] = '\n';
	io_buffer_b[9] = '\n';
	//select_dev(1);
	f = open("/xx9.txt", O_CREAT | O_RDWR); //5
//	//f = open(argv[1], O_CREAT | O_RDWR);
//	//for (i = 0; i < 1572864; i++)
//	for (i = 0; i < 1; i++)
//	{
//		if (i % 10000 == 0)
//		{
//			printf("count is %d \n",i);
//			check_free_mem();
//		}
		write(f,io_buffer_a,10);
//		//write(f,io_buffer_b,10);
//	}
	close(f);
	printf("end process \n");
	free(io_buffer_a);
	free(io_buffer_b);
	exit(0);
}
