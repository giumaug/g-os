#include ".h"

int socket(int socket_family, int socket_type, int protocol) 
{
	unsigned int params[2];

	params[0]=socket_type;
	params[1]=SYSCALL(28,params);
	return  params[1];
}

int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen)
{
	unsigned int params[4];

	params[0]=sockfd;
	params[1]=0;
	params[2]=((struct sockaddr_in*) addr)->sin_port;
	params[3]=SYSCALL(29,params);
	return  params[3];
}

int recvfrom(int sockfd, void* data,u32 data_len,int flags,struct sockaddr* src_addr,socklen_t* addrlen)
{
	unsigned int params[6];

	params[0]=sockfd;
	params[1]=&(((struct sockaddr_in*) addr)->sin_addr);
	params[2]=&(((struct sockaddr_in*) addr)->sin_port);
	params[3]=data;
	params[4]=data_len;
	params[5]=SYSCALL(30,params);
	return  params[5];
}

int sendto(int sockfd,void* data,u32  data_len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen)
{
	unsigned int params[6];

	params[0]=sockfd;
	params[1]=((struct sockaddr_in*) addr)->sin_addr;
	params[2]=((struct sockaddr_in*) addr)->sin_port;
	params[3]=data;
	params[4]=data_len;
	params[5]=SYSCALL(30,params);
	return  params[5];
}

int close(int sockfd)
{
	unsigned int params[2];

	params[0]=socket_type;
	params[1]=SYSCALL(32,params);
	return  params[1];
}

