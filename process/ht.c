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
void process_request_2(int client_sockfd);
void process_request_3(int client_sockfd);
void process_request_4(int client_sockfd);

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
	int age = 0;
	struct sockaddr_in ssock;
	
	ssock.sin_family = AF_INET;
//	((unsigned char*) &(server_address.sin_addr.s_addr))[0]=87;
//	((unsigned char*) &(server_address.sin_addr.s_addr))[1]=11;
//	((unsigned char*) &(server_address.sin_addr.s_addr))[2]=64;
//      ((unsigned char*) &(server_address.sin_addr.s_addr))[3]=111;

	((unsigned char*) &(server_address.sin_addr.s_addr))[0]=192;
	((unsigned char*) &(server_address.sin_addr.s_addr))[1]=168;
	((unsigned char*) &(server_address.sin_addr.s_addr))[2]=5;
    ((unsigned char*) &(server_address.sin_addr.s_addr))[3]=65;

	((unsigned char*) &(server_address.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(server_address.sin_port))[1]=((unsigned char*) &(port))[0];
//------------------------------------------------------------------------------
	
	int request_count = 0;
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(server_sockfd, (struct sockaddr *)&server_address,server_len);
	listen(server_sockfd, 5);
	char ch[100];
	int pid;
	while(1) 
	{
		age++;
		if ((age % 1000) == 0)
		{
			check_free_mem();
		}
		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);

		request_count++;
		if ((request_count % 1000) == 0)
		{
			printf("accepted request %d \n",request_count++);
		}

		if(fork() == 0) 
		{
			process_request(client_sockfd);
			//process_request_2(client_sockfd);
			//process_request_3(client_sockfd);
			close_socket(client_sockfd);
			exit(0);
		}
		else 
		{
			close_socket(client_sockfd);
		}
	}
	close_socket(server_sockfd);
	printf("ended !! \n");
	exit(0);
}

void process_request_3(int client_sockfd)
{
	t_stat stat_data;
	int get_index;
	char get[100];
	int ret = -1;
	int f = 0;
	unsigned char* io_buffer = NULL;
	unsigned int b_read = 0;
	unsigned int b_to_read = 4096;
	unsigned int file_size = 83424;  //32716
	const char path[] = "/usr/src/kernels/g-os/network/xxx.c";
	const char body[]  = "ciao";
	const char http_header[]  = "HTTP/1.1 200 OK\nConnection: close\nContent-Type:text/plain;\nContent-Disposition: inline;charset=utf-8\nContent-Length:83424\n\n";

	//stat(path,&stat_data);
	io_buffer = malloc(b_to_read + 1 );
	//get_index = read_socket(client_sockfd,(void*)get,100);
	ret = write_socket(client_sockfd,http_header,123);
	if (ret < 0)
	{
		close_socket(client_sockfd);
		exit(0);
	}
	f = open(path, O_RDWR | O_APPEND);
	if (f == -1)
	{
		printf("file not found\n");
		free(io_buffer);
		return;
	}
	int tot_read = 0;
	while (file_size > 0)
	{
		if ((int)(file_size - b_to_read) < 0)
		{
			b_to_read = file_size;
		}
		b_read = read(f,io_buffer,b_to_read);
		//b_read = b_to_read;
		ret = write_socket(client_sockfd,io_buffer,b_read);
		if (ret < 0)
		{
			close_socket(client_sockfd);
			exit(0);
		}
		file_size -= b_read;
		tot_read += b_read;
		//printf("tot read= %d \n",tot_read);
	}
	close(f);
	free(io_buffer);
}

void process_request_2(int client_sockfd)
{
	int ret = 0;
	const char body[]  = "ciao";
	const char http_header[]  = "HTTP/1.1 200 OK\nConnection: close\nContent-Type:text/html;\nContent-Disposition: inline;charset=utf-8\nContent-Length:5\n\n";

    printf("inside request 2 \n");
	ret = write_socket(client_sockfd,http_header,118);
	if (ret < 0)
	{
		close_socket(client_sockfd);
		exit(0);
	}
	ret = write_socket(client_sockfd,body,5);
	if (ret < 0)
	{
		close_socket(client_sockfd);
		exit(0);
	}
}

//GET /corriere.it/pippo.txt HTTP/1.1
void process_request(int client_sockfd)
{
	int ret = -1;
	char get[1000];
	char path[1000];
	char content_len[10];
    	char* io_buffer;
	t_stat stat_data;
	//struct stat stat_data;
	int index = 4;
	int f = 0;
	int i = 0;
	const char http_header_1[] = "HTTP/1.1 200 OK\nConnection: close\nContent-Type: ";
	const char http_header_2[] = ";\nContent-Disposition: inline;charset=utf-8\nContent-Length: ";
	const char root_path[] = "";
	const char text[] = "text/plain";
	const char html[] = "text/html";
	char http_header[2000];
	char* p = NULL;
	
	char* http_response = NULL;
	int http_header_len = 0;
	int http_body_len = 0;
	int root_path_len = 0;
	int b_read = 0;
	int b_to_read = 4096;

    char get2[1000];	
	int get_index;

	http_header_len = sizeof(http_header);
	root_path_len = sizeof(root_path) - 1;
	get_index = read_socket(client_sockfd,(void*)get,100);
	if (get_index < 0)
	{
		close_socket(client_sockfd);
		exit(0);
	}
	
	for (i = 0;i < get_index;i++)
	{
		get2[i] = get[i];
	}
	
	get2[get_index - 1] = '\0';
	
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
	stat(path,&stat_data);
	http_body_len = stat_data.st_size;
	itoa(http_body_len,content_len,10);
	io_buffer = malloc(b_to_read);
	http_response = malloc(http_body_len + http_header_len);

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

	f = open(path, O_RDWR | O_APPEND);
	if (f == -1)
	{
		free(io_buffer);
		free(http_response);
		return;
	}
	
	ret = write_socket(client_sockfd,http_response,b_read);
	if (ret < 0)
	{	
		close_socket(client_sockfd);
		exit(0);
	}
	
	while (http_body_len > 0)
	{
		if (http_body_len - b_to_read < 0)
		{
			b_to_read = http_body_len;
		}
		b_read = read(f,io_buffer,b_to_read);
		if (b_read < 0)
		{
			close_socket(f);
			exit(0);
		}
		ret = write_socket(client_sockfd,io_buffer,b_read);
//		printf("ret is %d \n" , ret);
		if (ret < 0)
	 	{
			close_socket(client_sockfd);
			exit(0);
		}
		http_body_len -= b_read;
	}
	ret = 100;
	while (ret > 0)
	{
		ret = read_socket(client_sockfd,io_buffer,10);
		//printf("ret is %d \n" , ret);
	}
	close(f);
	free(io_buffer);
	free(http_response);
}
void process_request_4(int client_sockfd)
{
	return;
}
