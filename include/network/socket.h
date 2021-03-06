#ifndef SOCKET_H                
#define SOCKET_H

#include "system.h"
#include "data_types/hashtable.h"
#include "data_types/queue.h"
#include "data_types/primitive_types.h"
#include "network/tcp.h"
#include "network/tcp_socket.h"

#define SOCKET_MAP_SIZE 100
#define TCP_MAP_SIZE 20
#define UDP_MAP_SIZE 20

struct s_spinlock_desc;
struct s_tcp_conn_desc;

typedef struct s_socket
{
	struct s_spinlock_desc lock;
	struct t_process_context* process_context;
	struct s_udp_conn_desc* udp_conn_desc;
	struct s_tcp_conn_desc* tcp_conn_desc;
	u32 type;
	u32 sd;
}
t_socket;

t_socket* socket_init(int type);
void socket_free(t_socket* socket);
int _open_socket(int type);
int _bind(int sockfd,u32 src_ip,u32 src_port,u32 dst_ip,u16 dst_port);
int _connect(int sockfd,u32 dst_ip,u16 dst_port);
int _listen(int sockfd);
int _accept(int sockfd);
int _recvfrom(int sockfd,unsigned char* src_ip,unsigned char* src_port,void* data,u32 data_len);
int _sendto(int sockfd,u32 dst_ip,u16 dst_port,void* data,u32 data_len);
int _close_socket(int sockfd);

#endif
