#include "lib/lib.h"

int main()
{
	int f;
	int b_read;
	char* io_buffer = NULL;

	select_dev(1);
	io_buffer = malloc(100);
	f = open("/test.txt", O_RDWR | O_APPEND);
	if (f == -1)
	{
		printf("file not found\n");
		return;
	}
	b_read = read_write(f, io_buffer, 20, 0);
	io_buffer[b_read] ='\0';
	close(f);
	printf("read string %s \n",io_buffer);	
	free(io_buffer);

/*
	select_dev(1);
	int sss = O_CREAT | O_RDWR;
	printf("sss = %d \n",sss);
	f = open("/xxxxx.txt", O_CREAT | O_RDWR);
	close(f);
	printf("end process \n");
*/
	exit(0);
}
