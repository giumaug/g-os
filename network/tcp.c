#define TCP_RCV_SIZE 
#define TCP_SND_SIZE

#define PORT_MAP_SIZE 20
void tcp_init()
{
	t_tcp_desc tcp_desc;
	
	tcp_desc=kmalloc(sizeof(t_tcp_desc));
	tcp_desc->rcv_port_map=hashtable_init(TCP_RCV_SIZE);

}


void rcv_packet_tcp(t_data_sckt_buf* data_sckt_buf)
{

}

typedef t_data_sckt_buf t_tcp_rcv_buf;
typedef t_data_sckt_buf s_tcp_snd_buf;

typedef struct s_tcp_conn_desc
{
	t_tcp_rcv_buf* tcp_rcv_buf;
	t_tcp_snd_buf* tcp_snd_buf;
}
t_tcp_conn_desc;

typedef struct s_tcp_desc
{
	t_hashtable* rcv_port_map;
}
t_tcp_desc



