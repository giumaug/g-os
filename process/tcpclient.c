//#include<stdio.h>
//#include<unistd.h>
//#include<sys/types.h>
//#include<string.h>
//#include<sys/socket.h>
//#include<arpa/inet.h>
//#include<netinet/in.h>

#include "lib/lib.h"
#include "tcpclient.h"

void main()
{
	int sid;
	char s[100]={},s1[100]={};
	
//-----------------LINUX------------------	
//	struct sockaddr_in ssock,csock;
//	sid=socket(AF_INET,SOCK_STREAM,0);
//	ssock.sin_family=AF_INET;
//	ssock.sin_addr.s_addr=inet_addr("192.168.237.124");
//	ssock.sin_port=htons(39734);
//----------------------------------------
	
//-----------------G-OS-------------------
	unsigned int ip=IP(172,16,6,1);
	unsigned int port=21846;
    	struct sockaddr_in ssock;
	
	ssock.sin_family = AF_INET;
	((unsigned char*) &(ssock.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(ssock.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(ssock.sin_addr.s_addr))[2]=6;
	((unsigned char*) &(ssock.sin_addr.s_addr))[3]=1;
//  	send_addr.sin_port = (unsigned short) port;
	((unsigned char*) &(ssock.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(ssock.sin_port))[1]=((unsigned char*) &(port))[0];
//----------------------------------------
	
	sid = socket(AF_INET, SOCK_STREAM, 0);	
	connect(sid,(struct sockaddr *) &ssock, sizeof(ssock));

	printf("\n Enter the string:");
	scanf("%s",s);
	printf("input = %s \n",s);
	int n = write(sid, s, strlen(s));
	printf("sent = %d \n",n);
	read_socket(sid,(void*)s1,sizeof(s1));
	printf("\n The receiveddd string is:%s\n",s1);
	close_socket(sid);
}