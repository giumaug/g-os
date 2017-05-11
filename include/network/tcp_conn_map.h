#ifndef TCP_CONN_MAP             
#define TCP_CONN_MAP

#include "network/tcp.h"

#define DPLC_CONN_MAP_SIZE 5

struct s_tcp_conn_desc;

typedef struct s_tcp_conn_map
{
	t_hashtable* conn_map;
	t_hashtable* duplicate_key_map;
	t_llist* duplicate_conn_list;
	unsigned char* zero_const;
	unsigned char* one_const;
}
t_tcp_conn_map;

t_tcp_conn_map* tcp_conn_map_init();
void tcp_conn_map_put(t_tcp_conn_map* tcp_conn_map,u16 src_ip,u16 dst_ip,u32 src_port,u32 dst_port,struct s_tcp_conn_desc* tcp_conn_desc);
void tcp_conn_map_remove(t_tcp_conn_map* tcp_conn_map,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port);
struct s_tcp_conn_desc* tcp_conn_map_get(t_tcp_conn_map* tcp_conn_map,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port);
void tcp_conn_map_free(t_tcp_conn_map* tcp_conn_map);

#endif

