/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include "lib/lib.h"
#include "udps.h"

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
	unsigned int port=21846;//simmetria byte ordering!!!!
	int sockfd; 				
  	int clientlen; 					
  	struct sockaddr_in serveraddr; 			
  	struct sockaddr_in clientaddr; 						
  	char buf[BUFSIZE]; 						
  	int n,i; 						

  	serveraddr.sin_family = AF_INET;
  	serveraddr.sin_addr.s_addr = INADDR_ANY;
//  	serveraddr.sin_port = (unsigned short) port;
	((unsigned char*) &(serveraddr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(serveraddr.sin_port))[1]=((unsigned char*) &(port))[0];

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  	if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
    		printf("ERROR on binding");

  	clientlen = sizeof(clientaddr);
  	for (i = 0; i < 4000000;i++)
	{
    		n = recvfrom(sockfd, buf, BUFSIZE, 0,(struct sockaddr *) &clientaddr, &clientlen);
		buf[n]='\0';
    		//printf("server received: %s \n",buf);
    		n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &clientaddr, clientlen);
		if (((i % 1000) == 0))
		{
			printf("age is %d \n",i);
			check_free_mem();
		}
	}
	printf("before \n");
	close_socket(sockfd);
	printf("after \n");
	exit(0);
}
