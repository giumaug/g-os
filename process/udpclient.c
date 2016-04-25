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
//	unsigned int ip=IP(172,16,6,1);
	unsigned int ip=IP(172,16,243,1);
	unsigned int port=21845;
	int sockfd,n_to,n_from;
    	int send_len;
	int rcv_len;
    	struct sockaddr_in send_addr;
	struct sockaddr_in rcv_addr;
    	char send_buf[BUFSIZE];
	char rcv_buf[BUFSIZE];

	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[2]=243;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[3]=1;
  	send_addr.sin_port = (unsigned short) port;

//	struct hostent *server;
//	server = gethostbyname("172.16.243.100");
//	bcopy((char *)server->h_addr,(char *)&serveraddr.sin_addr.s_addr, server->h_length);
	
	send_len = sizeof(send_addr);
	rcv_len = sizeof(rcv_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	while (1)
	{
		printf("type packet to send \n");
		scanf("%s",send_buf);
    		n_to = sendto(sockfd, send_buf, strlen(send_buf),0,&send_addr, &send_len);
   		n_from = recvfrom(sockfd, rcv_buf,n_to, 0, &rcv_addr,&rcv_len);
		rcv_buf[n_to]='\0';
		printf("\necho from server: %s\n",rcv_buf);
	}   
    	return 0;
}
