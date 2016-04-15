/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */

#include "lib/lib.h"
#include "udpserver.h"

#define BUFSIZE 1024

int main(int argc, char **argv) 
{
	int sockfd, portno, n;
    	int serverlen;
    	struct sockaddr_in serveraddr;
    	struct hostent *server;
    	char *hostname;
    	char buf[BUFSIZE];

    	if (argc != 3) 
	{
       		printf("usage: %s <hostname> <port>\n", argv[0]);
       		exit(0);
    	}
    	hostname = argv[1];
    	portno = atoi(argv[2]);

    	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    	if (sockfd < 0) 
        	printf("ERROR opening socket");

 
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

	serveraddr.sin_family = AF_INET;
  	serveraddr.sin_addr.s_addr = INADDR_ANY; // ------------------qui!!!!
  	serveraddr.sin_port = portno;



    /* get a message from the user */
    bzero(buf, BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, BUFSIZE, stdin);

    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
    if (n < 0) 
      error("ERROR in sendto");
    
    /* print the server's reply */
    n = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &serverlen);
    if (n < 0) 
      error("ERROR in recvfrom");
    printf("Echo from server: %s", buf);
    return 0;
}
