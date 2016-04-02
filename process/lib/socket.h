#ifndef SOCKET_H                
#define SOCKET_H

#define AF_INET  2

typedef unsigned short  sa_family_t;
typedef u32 socklen_t;
typedef int ssize_t;
typedef unsigned int ssize_t;

struct sockaddr 
{
	sa_family_t     sa_family;
	char            sa_data[6];
}

struct in_addr 
{
	u32   s_addr;
}

struct sockaddr_in 
{
	sa_family_t           sin_family;
	unsigned short int    sin_port;
	struct in_addr        sin_addr; 
 };

enum sock_type 
{
	SOCK_STREAM     = 1,
	SOCK_DGRAM      = 2,
	SOCK_RAW        = 3,
	SOCK_RDM        = 4,
	SOCK_SEQPACKET  = 5,
	SOCK_DCCP       = 6,
	SOCK_PACKET     = 10
};

int socket(int socket_family, int socket_type, int protocol);
int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
int recvfrom(int sockfd, void* data,u32 data_len,int flags,struct sockaddr* src_addr,socklen_t* addrlen);
int sendto(int sockfd,void* data,u32  data_len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
int close(int sockfd);
#endif
