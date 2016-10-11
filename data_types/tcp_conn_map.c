#define TCP_CONN_MAP_SIZE 10

typedef struct s_tcp_conn_map
{
	t_hashtable* conn_map;
	t_llist* duplicate_conn_list;
	u8 is_key_unique;
}
t_tcp_conn_map;

t_tcp_conn_map* tcp_conn_map_init()
{
	t_tcp_conn_map* tcp_conn_map;
	
	tcp_conn_map = kmalloc(sizeof(t_tcp_conn_map));
	tcp_conn_map->conn_map = hashtable_init(TCP_CONN_MAP_SIZE);
	tcp_conn_map->duplicate_conn_list = new_dllist();
	tcp_conn_map->is_key_unique = 0;
 }

void tcp_conn_map_put(t_tcp_conn_map* tcp_conn_map,t_tcp_conn_desc* tcp_conn_desc)
{
	t_tcp_conn_map* tmp;

	conn_id = tcp_conn_desc->dst_port | (tcp_conn_desc->src_port << 16);
	tmp = hashtable_get(tcp_conn_map->conn_map,conn_id);
	if (tmp != NULL)
	{
		qui!!!!!!!!!!!
	}	

}

tcp_conn_map_get
{

}

tcp_conn_map_free
{

}






