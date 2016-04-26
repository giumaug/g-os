/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include "lib/lib.h"
#include "udpserver.h"

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
	unsigned int port=21845;//simmetria byte ordering!!!!
	int sockfd; 				
  	int clientlen; 					
  	struct sockaddr_in serveraddr; 			
  	struct sockaddr_in clientaddr; 						
  	char buf[BUFSIZE]; 						
  	int n; 						

  	serveraddr.sin_family = AF_INET;
  	serveraddr.sin_addr.s_addr = INADDR_ANY; //da definire su lib.h 
//  	serveraddr.sin_port = (unsigned short) port;
	((unsigned char*) &(serveraddr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(serveraddr.sin_port))[1]=((unsigned char*) &(port))[0];

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  	if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
    		printf("ERROR on binding");

  	clientlen = sizeof(clientaddr);
  	while (1) 
	{
    		n = recvfrom(sockfd, buf, BUFSIZE, 0,(struct sockaddr *) &clientaddr, &clientlen);
		buf[n]='\0';
    		printf("server received: %s \n",buf);
    		n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &clientaddr, clientlen);
	}
}
