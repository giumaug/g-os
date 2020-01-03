#include "lib/lib.h"

int main()
{
	int f;
	int i;
	int b_read;
	char* io_buffer = NULL;

/*
	select_dev(1);
	io_buffer = malloc(4100);
	f = open("/zero.txt", O_RDWR | O_APPEND);
	if (f == -1)
	{
		printf("file not found\n");
		return;
	}
	for (i = 0; i < 7680; i++)
	{
		b_read = read_write(f, io_buffer, 4096, 0);
	}
	io_buffer[b_read] ='\0';
	close(f);
	printf("read string %s \n",io_buffer);	
	free(io_buffer);
*/

	select_dev(1);
	int sss = O_CREAT | O_RDWR;
	printf("sss = %d \n",sss);
	f = open("/xxxxx.txt", O_CREAT | O_RDWR);
	//f = open("/test.txt", O_RDWR);
	close(f);
	printf("end process \n");
	exit(0);
}
