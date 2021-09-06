#include "lib/lib.h"

void main()
{
	int f;
	int counter=0;
	int err=0;
	int n1=0;
	int sid;
	char s1[512]={};
	unsigned int tot_read = 0;
	struct sockaddr_in server_address;
	unsigned int port=21846;
	const char path[] = "/yyy.txt";

	((unsigned char*) &(server_address.sin_addr.s_addr))[0]=192;
	((unsigned char*) &(server_address.sin_addr.s_addr))[1]=168;
	((unsigned char*) &(server_address.sin_addr.s_addr))[2]=122;
	((unsigned char*) &(server_address.sin_addr.s_addr))[3]=100;

	((unsigned char*) &(server_address.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(server_address.sin_port))[1]=((unsigned char*) &(port))[0];

//------------------------LINUX--------------------------------
//	struct sockaddr_in ssock,csock;
//	//sid=socket(AF_INET,SOCK_STREAM,0);
//	ssock.sin_family=AF_INET;
//	ssock.sin_addr.s_addr=inet_addr("172.16.6.101");
//	ssock.sin_port=htons(21846);
//------------------------------------------------------------
	
	f = open(path, O_CREAT | O_RDWR);
	if (f == -1)
	{
		printf("file not found\n");
		exit(0);
	}
	printf("request number %d \n",counter++);
	sid=socket(AF_INET,SOCK_STREAM,0);
	err=connect(sid,(struct sockaddr *)&server_address,sizeof(server_address));
	do
	{ 
		n1 = read_socket(sid,(void*)s1,sizeof(s1));
		if (n1 > 0) 
		{
			write(f,s1,512);
			//printf("\n The receiveddd string is:%s\n",s1);
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
	close(f);
	close_socket(sid);
	exit(0);
}
