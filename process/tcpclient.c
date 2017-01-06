#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
void main()
{
	int sid;
	char s[100]={},s1[100]={};
	struct sockaddr_in ssock,csock;
	sid=socket(AF_INET,SOCK_STREAM,0);
	ssock.sin_family=AF_INET;
	ssock.sin_addr.s_addr=inet_addr("192.168.237.124");
	ssock.sin_port=htons(39734);
	
	connect(sid,(struct sockaddr *)&ssock,sizeof(ssock));
	printf("\n Enter the string:");
	scanf("%s",s);
	printf("input = %s \n",s);
	int n = write(sid, s, strlen(s));
	printf("sent = %d \n",n);
	read(sid,(void*)s1,sizeof(s1));
	printf("\n The receiveddd string is:%s\n",s1);
	close(sid);
}
