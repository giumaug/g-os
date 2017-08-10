#include "lib/lib.h"
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <stdio.h>
//#include <netinet/in.h>
//#include <signal.h>
//#include <unistd.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <stdlib.h>

int main()
{
	int f;
	int age=0;
	char* io_buffer = NULL;
	t_stat stat_data;
	//struct stat stat_data;
	const char path[] = "/usr/src/kernels/g-os/network/tcp.c";
	
	printf(".......\n");
	//for (f=0;f<3000000000;f++);

	//while (1)
	{
		printf("start stat \n");
		stat(path,&stat_data);
		printf("end stat %d  \n",stat_data.st_size);

		io_buffer = malloc(stat_data.st_size + 1);
		printf("start open \n");
		f = open(path, O_RDWR | O_APPEND);
		printf("end open \n");
		if (f == -1)
		{
			printf("file not found\n");
			free(io_buffer);
			return;
		}
		printf("start read \n");
		read(f,io_buffer,10);
		io_buffer[10] = '\0';
		printf("xxx= %s \n",io_buffer);	
		read(f,io_buffer,10);	
		io_buffer[10] = '\0';
		printf("xxx= %s \n",io_buffer);
		read(f,io_buffer,10);
		io_buffer[10] = '\0';
		printf("xxx= %s \n",io_buffer);	
		read(f,io_buffer,10);	
		io_buffer[10] = '\0';
		printf("xxx= %s \n",io_buffer);		
		read(f,io_buffer,10);
		io_buffer[10] = '\0';
		printf("xxx= %s \n",io_buffer);	
		read(f,io_buffer,10);	
		io_buffer[10] = '\0';
		printf("xxx= %s \n",io_buffer);
		read(f,io_buffer,10);
		io_buffer[10] = '\0';
		printf("xxx= %s \n",io_buffer);	
		read(f,io_buffer,10);	
		io_buffer[10] = '\0';
		printf("xxx= %s \n",io_buffer);		
		printf("end read \n");
		free(io_buffer);
		close(f);
		printf("--------------------------------age=%d \n",age++);
	}
	exit(0);
}
