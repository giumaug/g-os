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
	params[1]=((struct sockaddr_in*) addr)->sin_addr;
	params[2]=((struct sockaddr_in*) addr)->sin_port;
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

	int _sendto(t_socket_desc* socket_desc,int sockfd,void* data,u32 data_len)

}

int close(int sockfd)
{
	qui!!!
}

