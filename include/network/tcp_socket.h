#ifndef TCP_SOCKET_H              
#define TCP_SOCKET_H

#include "network/tcp.h"
#include "network/tcp_conn_map.h"

int bind_tcp(t_tcp_conn_desc* tcp_conn_desc,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port);
int listen_tcp(t_tcp_conn_desc* tcp_conn_desc);
t_tcp_conn_desc* accept_tcp(t_tcp_conn_desc* tcp_conn_desc);
void connect_tpc(t_tcp_conn_map* tcp_req_map,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port);
void close_tcp(t_tcp_conn_desc* tcp_conn_desc);
int dequeue_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len);
int enqueue_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len);

#endif
