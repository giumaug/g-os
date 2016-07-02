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
//#include <sys/stat.h>
//#include <fcntl.h>

#define PACKET_SIZE 100
#define BUFSIZE 1024
#define IP(a,b,c,d)	(d | (c<<8) | (b<<16) | (a<<24))

typedef unsigned int u32;
typedef unsigned short u16;

int main(int argc, char **argv) 
{
	int packet_size=0;
	int sockfd,send_len,i,n_to,port;
    	struct sockaddr_in send_addr;
	char* packet=NULL; 

	packet_size=(rand() % 200 + 1);
	packet=malloc(packet_size);
	for (i=0;i<packet_size;i++)
	{
		packet[i]='s';
	}
	printf("starting udp_writer.+.. \n");
	port=20000;

	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[2]=6;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[3]=1;
	((unsigned char*) &(send_addr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(send_addr.sin_port))[1]=((unsigned char*) &(port))[0];

	send_len = sizeof(send_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	for (i=0;i<2;i++)
	{
		n_to = sendto(sockfd,(packet),packet_size,0,&send_addr, send_len);
	}
	close_socket(sockfd);
	printf("ending udp_writer..++. \n");
    	exit(0);
}
