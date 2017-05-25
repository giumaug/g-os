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
	char s[] = "ping";
	char s1[100]={};
	unsigned int tot_read = 0;
	struct sockaddr_in ssock,csock;
	//sid=socket(AF_INET,SOCK_STREAM,0);
	ssock.sin_family=AF_INET;
	//ssock.sin_addr.s_addr=inet_addr("192.168.124.101");
	ssock.sin_addr.s_addr=inet_addr("172.16.6.101");
	ssock.sin_port=htons(21846);
	
	connect(sid,(struct sockaddr *)&ssock,sizeof(ssock));
	sid=socket(AF_INET,SOCK_STREAM,0);
	
	while (n < 1)
	//while(1)
	{
		printf("request number %d \n",counter++);
		do
		{ 
			n1 = read(sid,(void*)s1,sizeof(s1));
			if (n1 > 0) 
			{
				printf("\n The receiveddd string is:%s\n",s1);
				tot_read += n1; 				
			}
			printf("\n n1 is:%d\n",n1);
			printf("\n tot_read is:%d\n",tot_read);
		}
		while (n1 > 0);
		if (n1 < 0) 
		{
			printf("\n ERROR!!!!!!! \n");
			while(1);
		}
		close(sid);
		n++;
	}
}
