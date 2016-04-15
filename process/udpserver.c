/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include "lib/lib.h"
#include "udpserver.h"

#define BUFSIZE 1024

int main(int argc, char **argv) 
{
	int sockfd; 					/* socket */
  	int portno; 					/* port to listen on */
  	int clientlen; 					/* byte size of client's address */
  	struct sockaddr_in serveraddr; 			/* server's addr */
  	struct sockaddr_in clientaddr; 			/* client addr */
  	struct hostent *hostp; 				/* client host info */
  	char buf[BUFSIZE]; 				/* message buf */
  	char *hostaddrp; 				/* dotted decimal host addr string */
  	int n; 						/* message byte size */

  	if (argc != 2) 
	{
    		printf("usage: %s <port>\n", argv[0]);
    		exit(1);
  	}
  	portno = atoi(argv[1]);

  	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  	if (sockfd < 0) 
    		printf("ERROR opening socket");

  	serveraddr.sin_family = AF_INET;
  	serveraddr.sin_addr.s_addr = INADDR_ANY; // da definire
  	serveraddr.sin_port = portno;

  	if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
    		printf("ERROR on binding");

  	clientlen = sizeof(clientaddr);
  	while (1) 
	{
    		n = recvfrom(sockfd, buf, BUFSIZE, 0,(struct sockaddr *) &clientaddr, &clientlen);
    		if (n < 0)
      			printf("ERROR in recvfrom");

    		printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);
    
    		n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &clientaddr, clientlen);
    		if (n < 0) 
      			printf("ERROR in sendto");
	}
}
