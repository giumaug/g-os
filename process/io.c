//#include <sys/types.h>
//#include <sys/socket.h>
//#include <stdio.h>
//#include <netinet/in.h>
//#include <signal.h>
//#include <unistd.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <stdlib.h>

#include "lib/lib.h"

int main()
{
	//const char path[] = "/home/ubuntu/sample.txt";
	const char path[] = "/io";
	//const char path[] = "/shell";
	char* io_buffer;
	int current_len;
	int file_len;
	t_stat stat_data;
	//struct stat stat_data;
	int b_read,f;
	int b_to_read = 5120;
	int t = 0;
	
	printf("---------start performance check---------- \n");
	//read_test();
	printf("---------end performance check------------ \n");

	f = open(path, O_RDWR | O_APPEND);
	if (f == -1)
	{
		printf("file not found...++..\n");
		exit(0);
		return;
	}

	stat(path,&stat_data);
	file_len = stat_data.st_size;
	io_buffer = malloc(b_to_read);
	//file_len = 31457280;
	file_len = 29414;
	printf("file len is... %d \n",file_len);
	current_len = file_len;
	while (current_len > 0)
	{
		b_read = read(f,io_buffer,b_to_read);
		printf("byte read = %d \n",b_read);
		current_len -= b_read;
	}
	printf("----qui!!! \n");
	close(f);
	free(io_buffer);
	check_free_mem();
	exit(0);	
}
