#include "lib/lib.h"

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
#define SERVER_PORT 20000

int main(int _argc, char **_argv) 
{
	char** argv;
	char* writer="./udpwriter";
	unsigned int port=SERVER_PORT;
	int sockfd,clientlen,n,ret,pid; 									
  	struct sockaddr_in serveraddr; 			
  	struct sockaddr_in clientaddr; 						
  	char buf[BUFSIZE];
	char client_port[3];
	char cli_port[10]; 												

	argv=malloc(sizeof(char*)*4);
	argv[0]=writer;

  	serveraddr.sin_family = AF_INET;
  	serveraddr.sin_addr.s_addr = INADDR_ANY;
	((unsigned char*) &(serveraddr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(serveraddr.sin_port))[1]=((unsigned char*) &(port))[0];

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  	if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
    		printf("ERROR on binding");
	}

  	clientlen = sizeof(clientaddr);
  	while (1) 
	{
    		n = recvfrom(sockfd, buf, BUFSIZE, 0,(struct sockaddr *) &clientaddr, &clientlen);
		buf[n]='\0';
		client_port[0]=((unsigned char*) &(clientaddr.sin_port))[1];
		client_port[1]=((unsigned char*) &(clientaddr.sin_port))[0];
		client_port[2]=0;
		
		argv[1]=buf;
		argv[2]=client_port;
		argv[3]=0;

		printf("file is %s \n",argv[1]);
		printf("port is %d \n",clientaddr.sin_port);

		pid=fork();
		if (pid==0)
		{
			ret=exec(argv[0],argv);	
		}
	}
}
