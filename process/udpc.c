/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */

#include "lib/lib.h"
#include "udpc.h"

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

int main(int argc, char **argv) 
{
	unsigned int i,j,index;
	unsigned int port = 21846;
	int sockfd,n_to,n_from;
    int send_len;
	int rcv_len;
    struct sockaddr_in send_addr;
	struct sockaddr_in rcv_addr;
    char send_buf[BUFSIZE];
	char rcv_buf[BUFSIZE];

	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[0] = 192;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[1] = 168;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[2] = 5;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[3] = 60;

//  send_addr.sin_port = (unsigned short) port;
	((unsigned char*) &(send_addr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(send_addr.sin_port))[1]=((unsigned char*) &(port))[0];

	index = 0;
	for (i = 0;i < 3;i++)
	{
		for(j = 0;j < 10;j++)
		{
			send_buf[index] = 48 + j;
			index++;
		}
	}
	
	send_buf[index] = 48;
	index++;
	send_buf[index] = 49;
	index++;
	send_buf[index] = 50;
	index++;
	send_buf[index] = 51;
	index++;
	send_buf[index] = 52;
	index++;
	send_buf[index] = 53;
	index++;
	send_buf[index] = 54;
	index++;
	send_buf[index] = 55;
	index++;
	send_buf[index] = 56;
	index++;
	send_buf[index] = 57;
	index++;
	send_buf[index] = 58;
	
	send_buf[index] = '\0';	

	send_len = sizeof(send_addr);
	rcv_len = sizeof(rcv_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	for (i = 0; i < 5;i++)
	{
		n_to = sendto(sockfd, send_buf, strlen(send_buf),0,&send_addr, send_len);
   		//n_from = recvfrom(sockfd, rcv_buf,n_to, 0, &rcv_addr,rcv_len);
		//rcv_buf[n_to]='\0';
	}
	close_socket(sockfd);
    exit(0);
}
