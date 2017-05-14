#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
void main()
{
	int counter=0;
	int err=0;
	int n=0;
	int n1=0;
	int sid;
	char s[] = "0123456789";
	char s1[100]={};
	struct sockaddr_in ssock,csock;
	//sid=socket(AF_INET,SOCK_STREAM,0);
	ssock.sin_family=AF_INET;
	ssock.sin_addr.s_addr=inet_addr("192.168.124.101");
	ssock.sin_port=htons(21846);
	
	sid=socket(AF_INET,SOCK_STREAM,0);
	err=connect(sid,(struct sockaddr *)&ssock,sizeof(ssock));
	if (err!=0) 
	{
		printf("\n ERROR1!!!!!!! \n");
		while(1);
	}
	while(1)
	{
		err=write(sid, s, strlen(s));
		if (err<=0) 
		{
			printf("\n ERROR2!!!!!!! \n");
			while(1);
		}
		n1 = read(sid,(void*)s1,11);
		if (n1<=0) 
		{
			printf("\n ERROR!!!!!!! \n");
			while(1);
			exit(0);
		}
		printf("%s \n",s1);
	}
	close(sid);
}
