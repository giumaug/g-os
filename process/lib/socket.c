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
	unsigned char* port;
	unsigned int params[4];

	port=&((struct sockaddr_in*) addr)->sin_port;
	params[0]=sockfd;
	params[1]=0;
	params[2]=((port[0])<<8)+port[1];
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
	params[1]=(((unsigned char) ip[0]) <<24) + (((unsigned char) ip[1])<<16) + (((unsigned char) ip[2]) <<8) + (((unsigned char) ip[3]));
	params[2]=((port[0])<<8)+port[1];
	params[3]=data;
	params[4]=data_len;
	SYSCALL(31,params);
	return  params[5];
}

//symbol conflict!!!
int write_socket(int fd, void *buf, int count)
{
	unsigned int params[6];
	unsigned char* ip;
	unsigned char* port;

	params[0] = fd;
	params[1] = 0;
	params[2] = 0;
	params[3] = buf;
	params[4] = count;
	SYSCALL(31,params);
	return  params[5];------qui
}

//symbol conflict!!!
int read_socket(int fd, void *buf, int count)
{

}

//symbol conflict!!!
int close_socket(int sockfd)
{
	unsigned int params[1];

	params[0]=sockfd;
	SYSCALL(32,params);
	return  params[0];
}

