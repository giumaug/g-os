#ifndef TCP_SOCKET_H              
#define TCP_SOCKET_H

#include "network/tcp.h"
#include "network/tcp_conn_map.h"
#include "network/socket.h"

struct s_tcp_conn_desc;
struct s_tcp_conn_map;
struct s_socket;

int bind_tcp(struct s_tcp_conn_desc* tcp_conn_desc,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port);
int listen_tcp(struct s_tcp_conn_desc* tcp_conn_desc);
struct s_tcp_conn_desc* accept_tcp(struct s_tcp_conn_desc* tcp_conn_desc);
int connect_tcp(u32 dst_ip,u16 dst_port,struct s_socket* socket);
void close_tcp(struct s_tcp_conn_desc* tcp_conn_desc);
int dequeue_packet_tcp(struct s_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len);
int enqueue_packet_tcp(struct s_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len);

#endif
