#define TCP_CONN_MAP_SIZE 10

typedef struct s_tcp_conn_map
{
	t_hashtable* conn_map;
	t_llist* duplicate_conn_list;
	u8 is_key_unique;
}
t_tcp_conn_map;

t_tcp_conn_map* tcp_conn_map_init();
void tcp_conn_map_put(t_tcp_conn_map* tcp_conn_map,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port,t_tcp_conn_desc* tcp_conn_desc);
void tcp_conn_map_remove(t_tcp_conn_map* tcp_conn_map,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port);
void tcp_conn_map_get(t_tcp_conn_map* tcp_conn_map,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port);
void tcp_conn_map_free(t_tcp_conn_map* tcp_conn_map);



