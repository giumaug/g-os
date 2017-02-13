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
	//char s[100] = {};
	char s1[100] = {};
	char s[] = "ciao chicco!!!!!";

//-----------------LINUX------------------	
//	struct sockaddr_in ssock,csock;
//	sid=socket(AF_INET,SOCK_STREAM,0);
//	ssock.sin_family=AF_INET;
//	ssock.sin_addr.s_addr=inet_addr("192.168.237.124");
//	ssock.sin_port=htons(39734);
//----------------------------------------
	
//-----------------G-OS-------------------
	int msg_len;
	int index = 1;
	int i;
	unsigned int port=21846;
    	struct sockaddr_in ssock;
	
	ssock.sin_family = AF_INET;
//	((unsigned char*) &(ssock.sin_addr.s_addr))[0]=192;
//	((unsigned char*) &(ssock.sin_addr.s_addr))[1]=168;
//	((unsigned char*) &(ssock.sin_addr.s_addr))[2]=124;
//	((unsigned char*) &(ssock.sin_addr.s_addr))[3]=1;

	((unsigned char*) &(ssock.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(ssock.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(ssock.sin_addr.s_addr))[2]=6;
	((unsigned char*) &(ssock.sin_addr.s_addr))[3]=1;


//  	send_addr.sin_port = (unsigned short) port;
	((unsigned char*) &(ssock.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(ssock.sin_port))[1]=((unsigned char*) &(port))[0];
//----------------------------------------
	
	sid = socket(AF_INET, SOCK_STREAM, 0);
	printf("..\n");	
	connect(sid,(struct sockaddr *) &ssock, sizeof(ssock));
	sleep(5000);

//	while (1)
//	{
//		printf("\n Enter...+ the string:");
//		scanf("%s",s);
//		printf("input = %s \n",s);
//
//		msg_len = strlen(s);
//		s[msg_len] = '\n';
//		int n = write_socket(sid, s, (msg_len + 1));
//		sleep(5000);
//	}

	s[16]='\0';
	while (1)
	{
		for (i = 0;i <= index; i++)
		{
			int n = write_socket(sid, s,16);
		}
		index++;
		sleep(5000);
	}
	read_socket(sid,(void*)s1,sizeof(s1));
	printf("\n The receiveddd string is:%s\n",s1);
	close_socket(sid);
}
