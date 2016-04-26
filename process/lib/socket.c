#include "lib.h"

int socket(int socket_family, int socket_type, int protocol) 
{
	unsigned int params[2];

	params[0]=socket_type;
	SYSCALL(28,params);
	return  params[1];
}

int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen)
{
	unsigned int params[4];

	params[0]=sockfd;
	params[1]=0;
	params[2]=((struct sockaddr_in*) addr)->sin_port;
	SYSCALL(29,params);
	return  params[3];
}

int recvfrom(int sockfd, void* data,size_t data_len,int flags,struct sockaddr* addr,socklen_t* addrlen)
{
	unsigned int params[6];
	unsigned char* ip;
	unsigned char* port;

	ip=&((struct sockaddr_in*) addr)->sin_addr;
	port=&((struct sockaddr_in*) addr)->sin_port;

	params[0]=sockfd;
	params[1]=ip;
	params[2]=port;
	params[3]=data;
	params[4]=data_len;
	SYSCALL(30,params);

//	((unsigned char*) (((struct sockaddr_in*) addr)->sin_addr.s_addr))[0]= (*ip & 0xFF000000)>>24;
//	((unsigned char*) (((struct sockaddr_in*) addr)->sin_addr.s_addr))[1]= (*ip & 0xFF0000)>>16;
//	((unsigned char*) (((struct sockaddr_in*) addr)->sin_addr.s_addr))[2]= (*ip & 0xFF00)>>8;
//	((unsigned char*) (((struct sockaddr_in*) addr)->sin_addr.s_addr))[3]= (*ip & 0xFF);
//  	((struct sockaddr_in*) addr)->sin_port = (unsigned short) port;

	return  params[5];
}

int sendto(int sockfd,void* data,size_t  data_len, int flags,const struct sockaddr* addr, socklen_t addrlen)
{
	unsigned int params[6];
	unsigned char* ip;
	unsigned char* port;

	ip=&((struct sockaddr_in*) addr)->sin_addr;
	port=&((struct sockaddr_in*) addr)->sin_port;

	params[0]=sockfd;
//	params[1]=&((struct sockaddr_in*) addr)->sin_addr;
//	params[2]=&((struct sockaddr_in*) addr)->sin_port;
	params[1]=(((unsigned char) ip[0]) <<24) + (((unsigned char) ip[1])<<16) + (((unsigned char) ip[2]) <<8) + (((unsigned char) ip[3]));
	params[2]=((port[1]<<8)+(port[0]));
	params[3]=data;
	params[4]=data_len;
	SYSCALL(31,params);
	return  params[5];
}

//symbol conflict!!!
int close_socket(int sockfd)
{
	unsigned int params[1];

	SYSCALL(32,params);
	return  params[0];
}
