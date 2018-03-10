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
	const char path[] = "/sample2.txt";
	//const char path[] = "/io";
	//const char path[] = "/shell";
	unsigned char* io_buffer;
	int current_len;
	int file_len;
	t_stat stat_data;
	//struct stat stat_data;
	int b_read,f;
	int b_to_read = 4096;
	int t = 0;
	int i,j;
	unsigned long long hash = 0;
	int count = 0;

	unsigned  int vvv = (4294967290);
	printf("dddd = %d \n",vvv);
	while(1);
	
	printf("---------start performance check---------- \n");
	//read_test();
	printf("---------end performance check------------ \n");
	for (i = 0; i< 1; i++)
	{
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
		file_len = 31457280;
		//file_len = 29414;
		printf("file len is... %d \n",file_len);
		current_len = file_len;
		while (current_len > 0)
		{
			//hash = 0;
			count++;
			b_read = read(f,io_buffer,b_to_read);
			//printf("byte to read = %d \n",current_len);
			current_len -= b_read;
			for (j = 0; j < 4096;j++)
			{
				hash += io_buffer[j];
				//printf("val is %d \n",io_buffer[j]);	
			}
			printf("hash is %d \n",hash);
		}
		close(f);
		free(io_buffer);
		if (hash == 4011292438)
		{
			printf("hash is ok \n");
		}
		else
		{
			printf("hash is ko \n");
		}
		check_free_mem();
	}
	exit(0);	
}
