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
	unsigned int port=21845;
	int sockfd,n;
    	int serverlen;
    	struct sockaddr_in serveraddr;
    	char buf[BUFSIZE];

	serveraddr.sin_family = AF_INET;
	((unsigned char*) &(serveraddr.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(serveraddr.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(serveraddr.sin_addr.s_addr))[2]=6;
	((unsigned char*) &(serveraddr.sin_addr.s_addr))[3]=1;
  	serveraddr.sin_port = (unsigned short) port;

//	struct hostent *server;
//	server = gethostbyname("172.16.243.100");
//	bcopy((char *)server->h_addr,(char *)&serveraddr.sin_addr.s_addr, server->h_length);
	
	while (1)
	{
		printf("ip=%ud \n",ip);
		printf("type packet to send \n");
		scanf("%s",buf);

		serverlen = sizeof(serveraddr);
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    		n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
   		n = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &serverlen);
		buf[n]='\0';
		printf("echo from server;: %s\n",buf);
	}   
    	return 0;
}
