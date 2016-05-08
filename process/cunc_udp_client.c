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

#define BUFSIZE 1024
#define IP(a,b,c,d)	(d | (c<<8) | (b<<16) | (a<<24))

int main(int argc, char **argv) 
{
	unsigned int ip=IP(172,16,6,1);
//	unsigned int ip=IP(172,16,243,1);
	unsigned int port=21846;
	int sockfd,n_to,n_from;
    	int send_len;
	int rcv_len;
    	struct sockaddr_in send_addr;
	struct sockaddr_in rcv_addr;
    	char send_buf[BUFSIZE];
	char rcv_buf[BUFSIZE];
	char char_val[32];
	char* reader="/reader";
	u16 src_port=32768;
	
	argv=malloc(sizeof(char*)*2);
	argv[0]=reader;
	age=0;

	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[2]=243;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[3]=1;

	((unsigned char*) &(send_addr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(send_addr.sin_port))[1]=((unsigned char*) &(port))[0];

	
	send_len = sizeof(send_addr);
	rcv_len = sizeof(rcv_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	while (1)
	{
		n_request=(rand() % 10 + 1);
		printf("\nage=%d \n",age);
		printf("\nrequest number=%d \n",n_request);

		for (i=0;i<n_request;i++)
		{
			pid=fork();
			if (pid==0)
			{
				argv[1]=itoa(src_port++,argv,10);
				ret=exec(argv[0],argv);
			}
		}
		itoa(n_request,send_buf,10);
		n_to = sendto(sockfd, send_buf, strlen(send_buf),0,&send_addr, send_len);
		for (i=0;i<n_request;i++)
		{
   			n_from = recvfrom(sockfd, rcv_buf,n_to, 0, &rcv_addr,rcv_len);
		}
		age++;
		check_free_mem();
	}   
    	return 0;
}
