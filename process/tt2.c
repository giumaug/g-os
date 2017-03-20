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
	char s1[100] = {};
	char s[] = "ping";
	char buffer_1[16384];
	char buffer_2[16384];

//-----------------LINUX------------------	
//	struct sockaddr_in ssock,csock;
//	sid=socket(AF_INET,SOCK_STREAM,0);
//	ssock.sin_family=AF_INET;
//	ssock.sin_addr.s_addr=inet_addr("192.168.237.124");
//	ssock.sin_port=htons(39734);
//----------------------------------------
	
//-----------------G-OS-------------------
	int msg_len;
	int index = 16;
	int i,t;
	unsigned int port=21846;
    	struct sockaddr_in ssock;
	
	ssock.sin_family = AF_INET;
//	((unsigned char*) &(ssock.sin_addr.s_addr))[0]=192;
//	((unsigned char*) &(ssock.sin_addr.s_addr))[1]=168;
//	((unsigned char*) &(ssock.sin_addr.s_addr))[2]=124;
//      ((unsigned char*) &(ssock.sin_addr.s_addr))[3]=1;

	((unsigned char*) &(ssock.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(ssock.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(ssock.sin_addr.s_addr))[2]=6;
	((unsigned char*) &(ssock.sin_addr.s_addr))[3]=1;

	((unsigned char*) &(ssock.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(ssock.sin_port))[1]=((unsigned char*) &(port))[0];
//----------------------------------------
	
	for (t=0;t<16384;t++)
	{
		buffer_1[t] = *s;
	}

	sid = socket(AF_INET, SOCK_STREAM, 0);
	printf("..\n");	
	connect(sid,(struct sockaddr *) &ssock, sizeof(ssock));

//FIRST CLIENT TEST
	index = 10;
	while (1)
	for (t=0;t<2;t++)
	{
		for (i=0;i<index;i++)
		{
			buffer_2[i] = buffer_1[i];
		}
		sleep(5000);
		buffer_2[index]='\0';
		write_socket(sid, buffer_2,index);
		index += 16;
		if (index >4000)
		{
			index=4000;
		}
		printf("sent %d \n",index);
		//read_socket(sid,(void*)s1,sizeof(s1));
		//printf("\n The receiveddd string is:%s\n",s1);

	}

/*
//PING PONG TEST
//	while (1)
	for (t=0;t<1;t++)
	{
		sleep(5000);
		buffer_2[index]='\0';
		write_socket(sid,s,5);
		read_socket(sid,(void*)s1,sizeof(s1));
		//printf("\n The receiveddd string is:%s\n",s1);
		//while(1);
	}
	sleep(5000);
	printf("fin called!!!!! \n");
	close_socket(sid);
	//while(1);
	exit(0);
*/
}
//questo e' master
