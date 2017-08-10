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

void process_request(int client_sockfd);

int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	unsigned int data_sent = 0;

//-----------------------------LINUX--------------------------------------------
//	server_address.sin_family = AF_INET;//
//	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
//	server_address.sin_port = htons(9999);
//	server_len = sizeof(server_address);
//	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
//------------------------------------------------------------------------------

//-----------------G-OS---------------------------------------------------------
	int msg_len;
	int index = 16;
	int i,t;
	unsigned int port=21846;
	struct sockaddr_in ssock;
	
	ssock.sin_family = AF_INET;
//	((unsigned char*) &(server_address.sin_addr.s_addr))[0]=192;
//	((unsigned char*) &(server_address.sin_addr.s_addr))[1]=168;
//	((unsigned char*) &(server_address.sin_addr.s_addr))[2]=124;
//      ((unsigned char*) &(server_address.sin_addr.s_addr))[3]=101;

	((unsigned char*) &(server_address.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(server_address.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(server_address.sin_addr.s_addr))[2]=6;
        ((unsigned char*) &(server_address.sin_addr.s_addr))[3]=101;

	((unsigned char*) &(server_address.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(server_address.sin_port))[1]=((unsigned char*) &(port))[0];
//------------------------------------------------------------------------------
	
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(server_sockfd, (struct sockaddr *)&server_address,server_len);

	/* Create a connection queue, ignore child exit details and wait forclients. */

	listen(server_sockfd, 5);
	//signal(SIGCHLD, SIG_IGN);
	char ch[100];
	while(1) 
	{
		printf("http server waiting\n");

		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);

		printf("accepted request \n");
		if(fork() == 0) 
		{
			process_request(client_sockfd);
			close_socket(client_sockfd);
			//close(client_sockfd);
			exit(0);
		}
		else 
		{
			close_socket(client_sockfd);
			//close(client_sockfd);
		}
	}
	close_socket(server_sockfd);
	//close(client_sockfd);
	exit(0);
}

//GET /corriere.it/pippo.txt HTTP/1.1
void process_request(int client_sockfd)
{
	char get[100];
	char path[100];
	char content_len[10];
    	char* io_buffer;
	t_stat stat_data;
	//struct stat stat_data;
	int index = 4;
	int f = 0;
	int i = 0;
	const char http_header_1[] = "HTTP/1.1 200 OK\nConnection: close\nContent-Type: ";
	const char http_header_2[] = ";\nContent-Disposition: inline;charset=utf-8\nContent-Length: ";
	const char root_path[] = "/usr/src/kernels/g-os";
	const char text[] = "text/plain";
	const char html[] = "text/html";
	char http_header[200];
	char* p = NULL;
	
	char* http_response = NULL;
	int http_header_len = 0;
	int http_body_len = 0;
	int root_path_len = 0;
	int b_read = 0;
	int b_to_read = 4000;

	http_header_len = sizeof(http_header);
	root_path_len = sizeof(root_path) - 1;

	read_socket(client_sockfd,(void*)get,100);
	//read(client_sockfd,(void*)get,100);
	
	for (i = 0;i < root_path_len; i++)
	{
		path[i] = root_path[i];
	}
	do
	{
		path[index - 4 + root_path_len] = get[index];
	}
	while(get[index++] != ' ');
	int last_index = index - 5 + root_path_len;
	path[last_index] = '\0';

	if (path[last_index - 3]=='h' && path[last_index - 2]=='t' && path[last_index - 1]=='m')
	{
		p = html;
	}		
	else
	{
		p = text;
	}

	index = 0;
	do
	{
		http_header[index] = http_header_1[index];
	}
	while(http_header_1[index++] != '\0');
	
	i = 0;
	index--;
	do
	{
		http_header[index++] = p[i];
	}
	while(p[i++] != '\0');
		
	i = 0;
	index--;
	do
	{
		http_header[index++] = http_header_2[i];
	}
	while (http_header_2[i++] != '\0');
	http_header_len = index - 1;
	
	printf("start stat \n");
	stat(path,&stat_data);
	printf("end stat \n");
	http_body_len = stat_data.st_size + 1;
	itoa(http_body_len,content_len,10);
        //sprintf( content_len, "%d", http_body_len - 1);

	io_buffer = malloc(b_to_read);
	http_response = malloc(http_body_len + http_header_len);

	printf("start open \n");
	f = open(path, O_RDWR | O_APPEND);
	printf("end open \n");
	if (f == -1)
	{
		printf("file not found\n");
		free(io_buffer);
		free(http_response);
		return;
	}
//	printf("start read \n");
//	while (http_body_len > to_read)
//	{
//		read = read(f,io_buffer,http_body_len);
//		http_body_len -= to_read;
//	}
//	printf("end read \n");
//	close(f);
	
	for (i=0;i < http_header_len; i++)
	{
		http_response[i] = http_header[i];
	}
	index = 0;
	do
	{
		http_response[http_header_len + index - 1] = content_len[index];
	}
	while(content_len[index++] != '\0');
	index -= 2;
	http_response[http_header_len + index++] = '\n';
	http_response[http_header_len + index++] = '\n';
	b_read = http_header_len + index;

	printf("start open \n");
	f = open(path, O_RDWR | O_APPEND);
	printf("end open \n");
	if (f == -1)
	{
		printf("file not found\n");
		free(io_buffer);
		free(http_response);
		return;
	}
	//http_response[b_read] = '\0';
	printf("start read \n");
	write_socket(client_sockfd,http_response,b_read);
	http_body_len--;
	while (http_body_len > 0)
	{
		printf(".........\n");
		if (http_body_len - b_read < 0)
		{
			b_to_read = http_body_len;
		}
		b_read = read(f,io_buffer,b_to_read);
		write_socket(client_sockfd,io_buffer,b_read);
		printf("val is %d \n",b_read);
		printf("http_body_len=%d \n",http_body_len);
		//printf("io_buffer=%s \n",io_buffer);
	}
	write_socket(client_sockfd,'\0',1);
	printf("end read \n");
	close(f);
	close(client_sockfd);
	
//	http_header_len += index;
//	for(i = 0;i < http_body_len;i++)
//	{
//		http_response[http_header_len + i] = io_buffer[i];
//	}
//	http_response[http_header_len + http_body_len -1] = '\0';
//	//write_socket(client_sockfd,http_response,(http_header_len + http_body_len));
//	write(client_sockfd,http_response,(http_header_len + http_body_len - 1));
//	//printf("content is %s \n",http_response);
	free(io_buffer);
	free(http_response);
	sleep(1);
}
