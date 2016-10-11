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
	t_tcp_conn_desc* cur_conn;
	t_tcp_conn_desc* new_conn;

	conn_id = tcp_conn_desc->dst_port | (tcp_conn_desc->src_port << 16);
	cur_conn = hashtable_get(tcp_conn_map->conn_map,conn_id);
	if (cur_conn != NULL)
	{
		cur_conn->is_key_unique = 1;
	}	
	new_conn = tcp_conn_desc_int(tcp_conn_desc->src_port,tcp_conn_desc->dst_port);
	new_conn->is_key_unique = 1;
	ll_append(tcp_conn_map->duplicate_conn_list,new_conn);
}

t_tcp_conn_map* tcp_conn_map_get(t_tcp_conn_map* tcp_conn_map,u16 src_ip,u16 dst_ip,u32 src_ip,u32 dst_ip)
{
	u32 conn_id;
	t_tcp_conn_desc* tcp_conn_desc = NULL;
	
	conn_id = dst_port | (src_port << 16);
	tcp_conn_desc = hashtable_get(tcp_conn_map->conn_map,conn_id);

	if (tcp_conn_desc == NULL)
	{
		return NULL;
	}
	if (tcp_conn_desc->is_key_unique == 0)
	{
		return tcp_conn_desc;
	}
	
	sentinel_node = ll_sentinel(tcp_conn_map->duplicate_conn_list);
	next=ll_first(tcp_conn_map->duplicate_conn_list);
	while(next != sentinel_node && !stop)
	{
		next_process_context = next->val;
		quitttttttttttt-------------------------
	}
	

	duplicate_conn_list
}

tcp_conn_map_free
{

}






