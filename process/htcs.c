#include "lib/lib.h"

//#include <stdio.h>
//#include <unistd.h>
//#include <stdlib.h>
//#include <string.h>
//#include <netdb.h>
//#include <sys/types.h> 
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/stat.h>
//#include <fcntl.h>

int main(int argc, char **argv) 
{
	int iteration;
	char tmp[20];
	t_stat stat_data;
	int file_size;
	int path_size;
	int source_id;
	int len_read;
	int to_read = 4096;
	char get[1000];
	int sockfd,port,i;
    	struct sockaddr_in send_addr;
	const char get_part[] = " HTTP/1.1 \nUser-Agent: Wget/1.19.5 (linux-gnu) \nAccept: */* \n";
	char http_response[4200];
	int j,jj,f;
	int age = 0;
	int tot_of_tot = 0;
	
	port = 21846;
	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[0]=192;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[1]=168;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[2]=122;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[3]=100;
	((unsigned char*) &(send_addr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(send_addr.sin_port))[1]=((unsigned char*) &(port))[0];

	iteration = 2;
	printf("starting .... \n");

	//check_free_mem();
	for (j = 0; j < iteration;j++)
	{
		age++;
		//printf("age is... %d \n",age);

		if (((age % 100) == 0))
		{
			//check_free_mem();
		}
		f = open(argv[2], O_CREAT | O_RDWR);
		if (f == -1)
		{
			printf("file not found\n");
			exit(0);
		}
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		connect(sockfd,(struct sockaddr *) &send_addr, sizeof(send_addr));
		path_size = strlen(argv[1]);
		//path_size = 6;
		get[0] = 'G';
		get[1] = 'E';
		get[2] = 'T';
		get[3] = ' ';
		//printf("path size is... %d \n", path_size);
		for(i = 0; i <= path_size - 1;i++)
		{
			get[i + 4] = argv[1][i];
		}
		for(i = 0; i <= sizeof(get_part);i++)
		{
			get[i + path_size + 4] = get_part[i];
		}
		//printf("request is %s \n",get);
		write_socket(sockfd,get,(4 + sizeof(get_part) + path_size));
		len_read = read_socket(sockfd,http_response,124);//129//123
		//printf("len read is %d \n",len_read);
		//printf("res is %s \n",http_response);
		jj = 0;
		do
		{
			tmp[jj] = http_response[jj + 117];
			jj++; 
		}
		while(http_response[jj + 117] != '\n');
		tmp[jj] = '\0';
		file_size = atoi(tmp);
		file_size -= (len_read - 116 - jj - 3);

		tmp[0] = http_response[116 + jj + 3];
		tmp[1] = http_response[116 + jj + 4];
		tmp[2] = http_response[116 + jj + 5];
		tmp[3] = http_response[116 + jj + 6];
		tmp[4] = http_response[116 + jj + 7];
		tmp[5] = '\0';
	
		int val = 0;
		int tot = 0;
		while (file_size > 0)
		{
			len_read = read_socket(sockfd,http_response,to_read);
			tot += len_read;
			file_size -= len_read;
			write(f,http_response, len_read);
			//printf("len is %d \n",len_read);
			//printf("to read is %d \n", to_read);
		}
		tot_of_tot = tot_of_tot + tot;
		close_socket(sockfd);
		close(f);

		if (strcmp(argv[2],"/tcp.c") == 0 && tot != 32735)
		{
			printf("tcp.c ko \n");
			panic();
		}
		else if (strcmp(argv[2],"/scheduler.c") == 0 && tot != 14373)
		{
			printf("scheduler.c ko \n");
			panic();
		}
		else if (strcmp(argv[2],"/ext2.c") == 0 && tot != 34122)
		{
			printf("buddy.c ko \n");
			panic();
		}
		else if (strcmp(argv[2],"/vm.c") == 0 && tot != 16395)
		{
			printf("vm.c ko \n");
			panic();
		}
		else if (strcmp(argv[2],"/8042.c") == 0 && tot != 14562)
		{
			printf("tcp_socket.c ko \n");
			panic();
		}

		if (j < iteration - 1)
		{
			remove(argv[2]);
		}
	}
	//printf("name is %s ",argv[2]);
	printf("end \n");
   	exit(0);
}
