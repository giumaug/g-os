#include "data_types/tcp_conn_map.h"

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

void tcp_conn_map_remove(t_tcp_conn_map* tcp_conn_map,u16 src_ip,u16 dst_ip,u32 src_ip,u32 dst_ip)
{
	u32 conn_id;
	t_tcp_conn_desc* tcp_conn_desc = NULL;
	t_tcp_conn_desc* next_tcp_conn_desc = NULL;
	t_llist_node* sentinel_node = NULL;
	t_llist_node* next = NULL;
	u32 count = 0;
	
	conn_id = dst_port | (src_port << 16);
	tcp_conn_desc = hashtable_get(tcp_conn_map->conn_map,conn_id);

	if (tcp_conn_desc == NULL)
	{
		return;
	}
	if (tcp_conn_desc->is_key_unique == 0)
	{
		hashtable_remove(tcp_conn_map->conn_map,conn_id);
		return;
	}
	else if (tcp_conn_desc->src_ip == src_ip && tcp_conn_desc->dst_ip == dst_ip && tcp_conn_desc->src_port == src_port && tcp_conn_desc->dst_port == dst_port)
	{
		sentinel_node = ll_sentinel(tcp_conn_map->duplicate_conn_list);
		next=ll_first(tcp_conn_map->duplicate_conn_list);
		while(next != sentinel_node)
		{
			next_tcp_conn_desc = next->val;
			if (next_tcp_conn_desc->conn_id == conn_id)
			{
				count++;
			}
			if (count == 1)
			{
				remove_node = next;
			}
		}
		if (count == 1)
		{
			remove_node->val->is_unique_key = 0;
		}
		hashtable_remove(tcp_conn_map->conn_map,conn_id);
		hashtable_put(tcp_conn_map->conn_map,conn_id,remove_node->val);
		ll_delete_node(remove_node);	
	}
	else 
	{
		sentinel_node = ll_sentinel(tcp_conn_map->duplicate_conn_list);
		next = ll_first(tcp_conn_map->duplicate_conn_list);
		while(next != sentinel_node)
		{
			next_tcp_conn_desc = next->val;
			if (next_tcp_conn_desc->src_ip == src_ip && 
			    next_tcp_conn_desc->dst_ip == dst_ip && 
			    next_tcp_conn_desc->src_port == src_port && 
			    next_tcp_conn_desc->dst_port == dst_port)
			{
				remove_node = next;
			}

			if (next_tcp_conn_desc->conn_id == conn_id)
			{
				count++;
			}
		}
		ll_delete_node(remove_node);
		if (count == 1)
		{
			tcp_conn_desc->is_key_unique == 0
		}
	}
}

void tcp_conn_map_get(t_tcp_conn_map* tcp_conn_map,u16 src_ip,u16 dst_ip,u32 src_ip,u32 dst_ip)
{
	u32 conn_id;
	t_tcp_conn_desc* tcp_conn_desc = NULL;
	t_llist_node* sentinel_node = NULL;
	t_llist_node* next = NULL;
	
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
	else if (tcp_conn_desc->src_ip == src_ip && tcp_conn_desc->dst_ip == dst_ip && tcp_conn_desc->src_port == src_port && tcp_conn_desc->dst_port == dst_port)
	{
		return tcp_conn_desc;
	}
	else
	{
		sentinel_node = ll_sentinel(tcp_conn_map->duplicate_conn_list);
		next=ll_first(tcp_conn_map->duplicate_conn_list);
		while(next != sentinel_node)
		{
			tcp_conn_desc = next->val;
			if (tcp_conn_desc->conn_id == conn_id)
			{
				if (tcp_conn_desc->src_ip == src_ip && 
			    	    tcp_conn_desc->dst_ip == dst_ip && 
			   	    tcp_conn_desc->src_port == src_port && 
			    	    tcp_conn_desc->dst_port == dst_port)
				{
					return tcp_conn_desc;
				}
			}
		}
	}
}

void tcp_conn_map_free(t_tcp_conn_map* tcp_conn_map)
{
		hashtable_free(tcp_conn_map->conn_map);
		free_llist(tcp_conn_map->duplicate_conn_list);
		kfree(t_tcp_conn_map);
}
