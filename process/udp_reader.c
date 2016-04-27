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

#define SERVER_PORT 80
#define PACKET_SIZE 100
#define BUFSIZE 1024
#define IP(a,b,c,d)	(d | (c<<8) | (b<<16) | (a<<24))

int main(int argc, char **argv) 
{
	unsigned int ip=IP(172,16,6,1);
//	unsigned int ip=IP(172,16,243,1);
	int sockfd,n_to,n_from,send_len,port,rcv_len;
    	struct sockaddr_in send_addr;
	struct sockaddr_in rcv_addr;
    	char send_buf[BUFSIZE];
	char rcv_buf[BUFSIZE];	

	port=SERVER_PORT;
	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[2]=243;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[3]=1;
	((unsigned char*) &(send_addr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(send_addr.sin_port))[1]=((unsigned char*) &(port))[0];

	send_addr=argv[1];
	send_len = sizeof(send_addr);
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	n_to = sendto(sockfd, send_buf, strlen(send_buf),0,&send_addr, send_len);
	n_from = recvfrom(sockfd, rcv_buf,n_to, 0, &rcv_addr,rcv_len);
	rcv_buf[n_to]='\0';
	printf("rceceived data: %s \n",rcv_buf);

	close(sockfd);
	kfree???	   
    	exit(0);
}
