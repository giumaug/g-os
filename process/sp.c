//#include <sys/types.h>
//#include <sys/socket.h>
//#include <stdio.h>
//#include <netinet/in.h>
//#include <signal.h>
//#include <unistd.h>

#include "lib/lib.h"
#include "ke2.h"

int main()
{
	char s[] = "0123456789";
	char buffer_1[16384];
	char buffer_2[16384];
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	unsigned int data_sent = 0;
	int ret = -1;

//-----------------------------LINUX--------------------------------------------
//	server_address.sin_family = AF_INET;//
//	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
//	server_address.sin_port = htons(21846);
//	server_len = sizeof(server_address);
//	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
//------------------------------------------------------------------------------

//-----------------G-OS---------------------------------------------------------
	int rt =0;	
	int msg_len;
	int index = 16;
	int i,t,f;
	unsigned int port=21846;
    	struct sockaddr_in ssock;
	//const char path[] = "/usr/src/kernels/g-os/network/tcp.c";
	const char path[] = "/sample.txt";
	char* io_buffer;
	int current_len;
	int file_len;
	t_stat stat_data;
	int b_read;
	int b_to_read = 16384;
	int tt=0;

	ssock.sin_family = AF_INET;
//	((unsigned char*) &(server_address.sin_addr.s_addr))[0]=34;
//	((unsigned char*) &(server_address.sin_addr.s_addr))[1]=253;
//      ((unsigned char*) &(server_address.sin_addr.s_addr))[2]=28;
//      ((unsigned char*) &(server_address.sin_addr.s_addr))[3]=130;

	((unsigned char*) &(server_address.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(server_address.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(server_address.sin_addr.s_addr))[2]=6;
	((unsigned char*) &(server_address.sin_addr.s_addr))[3]=101;

	((unsigned char*) &(server_address.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(server_address.sin_port))[1]=((unsigned char*) &(port))[0];
//------------------------------------------------------------------------------

	for (t=0;t<16384;t++)
	{
		buffer_1[t] = *s;
	}	
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(server_sockfd, (struct sockaddr *)&server_address,server_len);
	listen(server_sockfd, 5);

	while(1) 
	{
		check_free_mem();
		printf("server waiting...\n");

		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);

		printf("accepted \n");
		if(fork() == 0) 
		{
			f = open(path, O_RDWR | O_APPEND);
			if (f == -1)
			{
				printf("file not found\n");
				return;
			}
			//stat(path,&stat_data);
			//file_len = stat_data.st_size;
			io_buffer = malloc(b_to_read);
			file_len = 31457280;
			printf("file len is %d \n",file_len);
			//b_to_read = file_len;
			//for (t=0;t<1000;t++)
			for (t=0;t<1;t++)
			{
				current_len = file_len;
				while (current_len > 0)
				{
					b_read = read(f,io_buffer,b_to_read);
					//printf("count= %d \n",b_read);
					ret = write_socket(client_sockfd,io_buffer,b_read);
					ret = 0;
					while (ret !=0 )
					{
						ret = write_socket(client_sockfd, buffer_2,index);	
						sleep(10);
						rt++;
						printf("retry=%d \n",rt);
					}				
					current_len -= b_read;
					io_buffer[b_read] = '\0';
				}
				lseek(f,0,SEEK_SET);
			}
			printf("rt=%d \n",rt);
			close_socket(client_sockfd);
			exit(0);
		}
		else 
		{
			close_socket(client_sockfd);
		}
	}
	close_socket(server_sockfd);
	exit(0);
}