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
	((unsigned char*) &(server_address.sin_addr.s_addr))[0]=79;
	((unsigned char*) &(server_address.sin_addr.s_addr))[1]=12;
	((unsigned char*) &(server_address.sin_addr.s_addr))[2]=212;
        ((unsigned char*) &(server_address.sin_addr.s_addr))[3]=127;

//	((unsigned char*) &(server_address.sin_addr.s_addr))[0]=172;
//	((unsigned char*) &(server_address.sin_addr.s_addr))[1]=16;
//	((unsigned char*) &(server_address.sin_addr.s_addr))[2]=6;
//      ((unsigned char*) &(server_address.sin_addr.s_addr))[3]=101;

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
		printf("http server waiting\n");
		check_free_mem();
		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);

		printf("accepted request %d \n",request_count++);
		if(fork() == 0) 
		{
			//printf("--1 \n");
			process_request(client_sockfd);
			//printf("--2 \n");
			close_socket(client_sockfd);
			//printf("--3 \n");
			//close(client_sockfd);
			exit(0);
		}
		else 
		{
			//printf("++++ \n");
			close_socket(client_sockfd);
			//close(client_sockfd);
		}
	}
	close_socket(server_sockfd);
	//close(client_sockfd);
	exit(0);
}

void process_request_3(int client_sockfd)
{
	int f = 0;
	unsigned char* io_buffer = NULL;
	unsigned int b_read = 0;
	unsigned int b_to_read = 4096;
	unsigned int file_size = 32617;
	const char path[] = "/usr/src/kernels/g-os/network/tcp.c";
	const char body[]  = "ciao";
	const char http_header[]  = "HTTP/1.1 200 OK\nConnection: close\nContent-Type:text/plain;\nContent-Disposition: inline;charset=utf-8\nContent-Length:32617\n\n";

	io_buffer = malloc(b_to_read + 1 );
	write_socket(client_sockfd,http_header,123);

	f = open(path, O_RDWR | O_APPEND);
	if (f == -1)
	{
		printf("file not found\n");
		free(io_buffer);
		return;
	}
	while (file_size > 0)
	{
		if ((int)(file_size - b_to_read) < 0)
		{
			b_to_read = file_size;
		}
		b_read = read(f,io_buffer,b_to_read);
		//io_buffer[b_read + 1] = '\0';
		//printf("data %s \n",io_buffer);
		write_socket(client_sockfd,io_buffer,b_read);
		file_size -= b_read;
	}
	close(f);
	free(io_buffer);
}

void process_request_2(int client_sockfd)
{
	const char body[]  = "ciao";
	const char http_header[]  = "HTTP/1.1 200 OK\nConnection: close\nContent-Type:text/html;\nContent-Disposition: inline;charset=utf-8\nContent-Length:5\n\n";

	write_socket(client_sockfd,http_header,118);
	write_socket(client_sockfd,body,5);
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
	int b_to_read = 4096;

        char get2[100];	
	int get_index;

	http_header_len = sizeof(http_header);
	root_path_len = sizeof(root_path) - 1;
	get_index = read_socket(client_sockfd,(void*)get,100);
	//read(client_sockfd,(void*)get,100);
	
	for (i = 0;i < get_index;i++)
	{
		get2[i] = get[i];
	}
	get2[get_index - 1] = '\0';
	//printf("get index is %d \n",get_index);
	//printf("get2 is %s \n",get2);
	
	for (i = 0;i < root_path_len; i++)
	{
		path[i] = root_path[i];
	}
	do
	{
		path[index - 4 + root_path_len] = get[index];
	}
	while(get[index++] != ' ');

	//printf("root_path_len %d \n",root_path_len);
	//printf("index--1 %d \n",index);


	int last_index = index - 5 + root_path_len;
	path[last_index] = '\0';
	//printf("index is %d \n",index);
	//printf("path is %s \n",path);

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
	
	//printf("index--2 %d \n",index);

	i = 0;
	index--;
	do
	{
		http_header[index++] = p[i];
	}
	while(p[i++] != '\0');

	//printf("index--3 %d \n",index);
	//printf("i---1 %d \n",i);
		
	i = 0;
	index--;
	do
	{
		http_header[index++] = http_header_2[i];
	}
	while (http_header_2[i++] != '\0');

	//printf("index--4 %d \n",index);
	//printf("i---2 %d \n",i);


	http_header_len = index - 1;
	
	int xxx = stat(path,&stat_data);
	//printf("stat size is %d \n",xxx);
	//http_body_len = stat_data.st_size + 1;
	http_body_len = stat_data.st_size;
	itoa(http_body_len,content_len,10);
	//printf("contet len %s \n",content_len);
        //sprintf( content_len, "%d", http_body_len - 1);

	//printf("path is %s \n",path);
	//printf("malloc 1 %d \n",b_to_read);
	io_buffer = malloc(b_to_read);
	//printf("malloc 2-1 %d \n",(http_body_len + http_header_len));
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
		printf("file not found\n");
		free(io_buffer);
		free(http_response);
		return;
	}
	write_socket(client_sockfd,http_response,b_read);
	//printf("body len is %d \n",http_body_len);
	while (http_body_len > 0)
	{
		if (http_body_len - b_to_read < 0)
		{
			b_to_read = http_body_len;
		}
		b_read = read(f,io_buffer,b_to_read);
		write_socket(client_sockfd,io_buffer,b_read);
		http_body_len -= b_read;
		//printf("val is %d \n",b_read);
		//printf("http_body_len= %d \n",http_body_len);
		//io_buffer[b_read] = '\0';------------------------------bug!!!!!!!!!!!
		//printf("io_buffer=%s \n",io_buffer);
	}
	//write_socket(client_sockfd,'\0',1);
	close(f);
	
	free(io_buffer);
	free(http_response);
}
