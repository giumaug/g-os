/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */

#include "lib/lib.h"
#include "udpclient.h"

//#include <stdio.h>
//#include <unistd.h>
//#include <stdlib.h>
//#include <string.h>
//#include <netdb.h>
//#include <sys/types.h> 
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

#define FIXED_PORT 21846
#define PACKET_SIZE 100
#define BUFSIZE 1024
#define IP(a,b,c,d)	(d | (c<<8) | (b<<16) | (a<<24))

int main(int argc, char **argv) 
{
	unsigned int ip=IP(172,16,6,1);
//	unsigned int ip=IP(172,16,243,1);
	int sockfd,sockfd_1,n_to;
    	int send_len,send_len_1;
    	struct sockaddr_in send_addr;
	struct sockaddr_in rcv_addr;
    	char send_buf[BUFSIZE];
	char send_buf_1[1];
	int i,f;
        char* io_buffer;
	t_stat stat_data;
	u16 port;
	u16 fixed_port;
	u32 data_len;
	u32 len_to_send;
	u32 offset;
	u32 sent;

	port=atoi(argv[0]);
	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[2]=243;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[3]=1;
	((unsigned char*) &(send_addr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(send_addr.sin_port))[1]=((unsigned char*) &(port))[0];

	fixed_port=FIXED_PORT;
	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr_1.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(send_addr_1.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(send_addr_1.sin_addr.s_addr))[2]=243;
	((unsigned char*) &(send_addr_1.sin_addr.s_addr))[3]=1;
	((unsigned char*) &(send_addr_1.sin_port))[0]=((unsigned char*) &(fixed_port))[1];
	((unsigned char*) &(send_addr_!.sin_port))[1]=((unsigned char*) &(fixed_port))[0];
	
	stat(argv[1],&stat_data);
	data_len=stat_data.st_size+1;
	io_buffer=malloc(data_len);
	f=open(argv[1], O_RDWR | O_APPEND);
	read(f,io_buffer,data_len);
	close(f);
	io_buffer[data_len]='\0';
	printf(io_buffer);
	free(io_buffer);

	send_len = sizeof(send_addr);
	send_len_1 = sizeof(send_addr_1);
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	sent=0;
	while (!sent)
	{
		if (data_len-(offset+PACKET_SIZE)>=0)
		{
			offset+=PACKET_SIZE;
			len=PACKET_SIZE;
		}
		else
		{
			len=data_len-offset;
			sent=1;
		}
    		n_to = sendto(sockfd, send_buf, strlen(send_buf),0,&send_addr, send_len);
	
		sockfd_1 = socket(AF_INET, SOCK_DGRAM, 0);
		n_to = sendto(sockfd_1, send_buf_1, strlen(send_buf_1),0,&send_addr_1, send_len_1);
		close(sockfd_1);
	}
	close(sockfd);
	kfree???	   
    	exit(0);
}
