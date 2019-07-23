#ifndef UDP_H                
#define UDP_H

#include "network/network.h"

#define UDP_EPMRL_PORT_MAP_SIZE 20
#define UDP_CONN_MAP_SIZE 20

typedef struct s_udp_desc
{
	u32 ep_port_index;
	struct s_hashtable* ep_port_map;
	struct s_hashtable* conn_map;
	t_spinlock_desc lock;
}
t_udp_desc;

typedef struct s_udp_conn_desc
{
	u16 src_port;
	u16 dst_port;
	u32 dst_ip;
	u32 src_ip;
	t_queue* rx_queue;
}
t_udp_conn_desc;

struct s_data_sckt_buf;

int udp_free_port_search();
t_udp_desc* udp_init();
void udp_free(t_udp_desc* udp_desc);
int send_packet_udp(u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port,char* data,u16 data_len);
void rcv_packet_udp(struct s_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len);
void udp_free(t_udp_desc* udp_desc);

#endif
