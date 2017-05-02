#include "network/socket.h"

//NON SERVONO SOLO PER COMPILARE UDP 
//static int free_port_search
//t_socket_desc* socket_desc_init()
//void socket_desc_free(t_socket_desc* socket_desc)

static int free_port_search()
{
	int i;
	void* port=NULL;
	t_socket_desc* socket_desc=NULL;

	socket_desc=system.network_desc->socket_desc;
	for (i=0;i<32767;i++)
	{
		if (socket_desc->udp_port_indx++>65535) 
		{
			
			socket_desc->udp_port_indx=32767;
		}
		port=hashtable_get(socket_desc->udp_map,socket_desc->udp_port_indx);
		if (port==NULL)
		{
			return socket_desc->udp_port_indx;
		}
	}
	return 0;
}

t_socket_desc* socket_desc_init()
{
	t_socket_desc* socket_desc=kmalloc(sizeof(t_socket_desc));
	socket_desc->sd_map=dc_hashtable_init(SOCKET_MAP_SIZE,&socket_free);
	socket_desc->tcp_map=hashtable_init(TCP_MAP_SIZE);
	socket_desc->udp_map=hashtable_init(UDP_MAP_SIZE);
	socket_desc->fd=0;
	socket_desc->udp_port_indx=32767;
	socket_desc->tcp_port_indx=0;
	SPINLOCK_INIT(socket_desc->lock);
	return socket_desc;
}

void socket_desc_free(t_socket_desc* socket_desc)
{
	hashtable_free(socket_desc->sd_map);
	hashtable_free(socket_desc->tcp_map);
	hashtable_free(socket_desc->udp_map);
	kfree(socket_desc);
	socket_desc->sd_map=NULL;
	socket_desc->tcp_map=NULL;
	socket_desc->udp_map=NULL;
}

t_socket* socket_init(int type)
{
	t_socket* socket=NULL;
	
	socket=kmalloc(sizeof(t_socket));
	socket->port = NULL;
	socket->ip = NULL;
	socket->process_context = NULL;

	if (type==2)
	{
		socket->udp_rx_queue=dc_new_queue(&free_sckt);
		socket->lock=kmalloc(sizeof(t_spinlock_desc));
		SPINLOCK_INIT(*socket->lock);
	}
	return 	socket;
}

//CALLED AT THE END SO LOCK NOT REQUIRED
void socket_free(t_socket* socket)
{
	if (socket->type == 2)
	{
		free_queue(socket->udp_rx_queue);
		kfree(socket->lock);
		kfree(socket);
	}
	else  if (socket->type == 1)
	{
		socket->tcp_conn_desc->ref_count--;
		//printk("---------\n");
		//printk("status=%d \n",socket->tcp_conn_desc->status);
		printk("ref_count=%d \n",socket->tcp_conn_desc->ref_count);
		if ((socket->tcp_conn_desc->status == ESTABILISHED || socket->tcp_conn_desc->status == CLOSE_WAIT) && socket->tcp_conn_desc->ref_count == 0)
		{
			close_tcp(socket->tcp_conn_desc);
		}
		kfree(socket);
	}
} 
/* NOTE:_open_socket,_bind,_connect,_listen,_accept,_rcvfrom,_send_to,_close_socket,
	 are free lock because process context keeps duplicated copy of socket.All possible
	 race is in tcp_conn_desc that is shared.Race on tcp_conn_desc is avoided disabling 
	 interrupt on tcp system call.There are no other way because this is a race beetwen
	 exception and interrupt path.
*/

int _open_socket(int type) 
{
	t_socket* socket = NULL;
	struct t_process_context* process_context = NULL;

	CURRENT_PROCESS_CONTEXT(process_context);
	socket = socket_init(type);
	socket->process_context = NULL;
	socket->ip = NULL;
	socket->port = NULL;
	socket->type = type;
	socket->sd = ++process_context->next_sd;
	hashtable_put(process_context->socket_desc,socket->sd,socket);
	return socket->sd;
}

int _bind(int sockfd,u32 src_ip,u32 src_port,u32 dst_ip,u16 dst_port)
{
	struct t_process_context* process_context = NULL;
	t_tcp_conn_desc* tcp_conn_desc = NULL;
	void* port;
	t_socket* socket=NULL;
	int ret=-1;
	
	CURRENT_PROCESS_CONTEXT(process_context);
	socket = hashtable_get(process_context->socket_desc,sockfd);
	if (socket != NULL)
	{
		if (socket->type == 2)
		{
//			//UDP CODE REFACTOR NEEDED!!!!!
//			if (hashtable_get(socket_desc->udp_map,src_port) == NULL)
//			{
//				socket->port = src_port;
//				hashtable_put(socket_desc->udp_map,src_port,socket);
//				ret = 0;
//			}
		}
		if (socket->type == 1)
		{
			tcp_conn_desc = tcp_conn_desc_int();
			socket->tcp_conn_desc = tcp_conn_desc;
			bind_tcp(socket->tcp_conn_desc,src_ip,dst_ip,src_port,dst_port);
		}
	}
	return ret;
}

int _connect(int sockfd,u32 dst_ip,u16 dst_port)
{
	struct t_process_context* process_context = NULL;
	t_socket* socket=NULL;
	int ret=-1;

	CURRENT_PROCESS_CONTEXT(process_context);
	socket = hashtable_get(process_context->socket_desc,sockfd);
	if (socket != NULL)
	{
		if (socket->type == 1)
		{
			connect_tcp(dst_ip,dst_port,socket);
		}
	}
	return ret;
}

int _listen(int sockfd)
{
	struct t_process_context* process_context = NULL;
	t_socket* socket=NULL;
	int ret=-1;

	CURRENT_PROCESS_CONTEXT(process_context);
	socket = hashtable_get(process_context->socket_desc,sockfd);
	if (socket != NULL)
	{
		if (socket->type == 1)
		{
			listen_tcp(socket->tcp_conn_desc);
		}
	}
	return ret;
}

int _accept(int sockfd)
{
	t_socket* socket = NULL;
	t_socket* new_socket = NULL;
	t_tcp_conn_desc* new_tcp_conn_desc = NULL;
	struct t_process_context* process_context = NULL;

	int ret=-1;
	CURRENT_PROCESS_CONTEXT(process_context);

	socket = hashtable_get(process_context->socket_desc,sockfd);
	if (socket != NULL)
	{
		if (socket->type == 1)
		{
			new_tcp_conn_desc = accept_tcp(socket->tcp_conn_desc);
			if (new_tcp_conn_desc != NULL)
			{
				new_socket = socket_init(1);
				new_socket->type = 1;
				new_socket->sd = ++process_context->next_sd;
				hashtable_put(process_context->socket_desc,new_socket->sd,new_socket);
				new_socket->tcp_conn_desc = new_tcp_conn_desc;
				ret = new_socket->sd;
			}
		}
	}
	return ret;
}

int _recvfrom(int sockfd,unsigned char* src_ip,unsigned char* src_port,void* data,u32 data_len)
{
	u32 read_data = 0;
	t_socket* socket = NULL;
	t_data_sckt_buf* data_sckt_buf = NULL;
	unsigned int _src_ip;
	unsigned int _src_port;
	struct t_process_context* process_context = NULL;

	CURRENT_PROCESS_CONTEXT(process_context);
	socket = hashtable_get(process_context->socket_desc,sockfd);
	if (socket != NULL)
	{
		if (socket->type == 2)
		{
//			SPINLOCK_LOCK(*socket->lock);
//
//			_src_port = GET_WORD(data_sckt_buf->transport_hdr[0],data_sckt_buf->transport_hdr[1]);
//			_src_ip = GET_DWORD(data_sckt_buf->network_hdr[12],data_sckt_buf->network_hdr[13],data_sckt_buf->network_hdr[14],data_sckt_buf->network_hdr[15]);
//			read_data = GET_WORD(data_sckt_buf->transport_hdr[4],data_sckt_buf->transport_hdr[5])-HEADER_UDP;
//
//			src_ip[0] = (_src_ip & 0xFF000000)>>24;
//			src_ip[1] = (_src_ip & 0xFF0000)>>16;
//			src_ip[2] = (_src_ip & 0xFF00)>>8;
//			src_ip[3] = (_src_ip & 0xFF);
//
//			src_port[0] = (_src_port & 0xFF00)>>8;
//			src_port[1] = (_src_port & 0xFF);
//
//			data_sckt_buf = dequeue(socket->udp_rx_queue);
//			if (data_sckt_buf == NULL)
//			{
//				_sleep_and_unlock(socket->lock);
//				SPINLOCK_LOCK(*socket->lock);
//				data_sckt_buf = dequeue(socket->udp_rx_queue);
//				SPINLOCK_UNLOCK(*socket->lock);
//				if (data_sckt_buf == NULL) 
//				{
//					return read_data;
//				}	
//			} 
//			if (read_data > data_len) 
//			{
//				read_data = data_len;
//			}
//			kmemcpy(data,data_sckt_buf->transport_hdr+HEADER_UDP,read_data);
//			free_sckt(data_sckt_buf);
		}
		else if (socket->type == 1)
		{
			read_data = dequeue_packet_tcp(socket->tcp_conn_desc,data,data_len);
		}
	}
	return read_data;
}

int _sendto(int sockfd,u32 dst_ip,u16 dst_port,void* data,u32 data_len)
{
	t_socket* socket=NULL;
	t_data_sckt_buf* data_sckt_buf=NULL;
	char* ip_payload=NULL;
	int ret=0;
	struct t_process_context* process_context = NULL;
	
	CURRENT_PROCESS_CONTEXT(process_context);
	socket=hashtable_get(process_context->socket_desc,sockfd);
	if (socket!=NULL)
	{
		if (socket->type==2)
		{
//			if (socket->port==0) 
//			{
//				socket->port=free_port_search();
//				socket_desc=system.network_desc->socket_desc;
//				SPINLOCK_LOCK(socket_desc->lock);
//				hashtable_put(socket_desc->udp_map,socket->port,socket);
//				SPINLOCK_UNLOCK(socket_desc->lock);
//			}
//			data_sckt_buf=alloc_sckt(data_len+HEADER_ETH+HEADER_IP4+HEADER_UDP);
//			data_sckt_buf->transport_hdr=data_sckt_buf->data+HEADER_ETH+HEADER_IP4;
//			ip_payload=data_sckt_buf->transport_hdr+HEADER_UDP;
//			kmemcpy(ip_payload,data,data_len);
//			ret=send_packet_udp(data_sckt_buf,system.network_desc->ip,dst_ip,socket->port,dst_port,data_len);
//
//			if (ret==0)
//			{
//				ret=data_len;
//			}
		}
		else if (socket->type==1)
		{
			enqueue_packet_tcp(socket->tcp_conn_desc,data,data_len);
		}
	}
	return ret;
}

//servono dei lock pure qui!!!!
int _close_socket(int sockfd)
{
	struct t_process_context* process_context = NULL;
	t_data_sckt_buf* data_sckt_buf=NULL;
	t_socket* socket=NULL;

	CURRENT_PROCESS_CONTEXT(process_context);
	socket=hashtable_remove(process_context->socket_desc,sockfd);
	if (socket->type==2)
	{
//		hashtable_remove(socket_desc->udp_map,socket->port);
	}
	if (sockfd == 1 )
	{
		printk("final!!! \n");--------------__!!!!
	}
	socket_free(socket);
}

t_hashtable* clone_socket_desc(t_hashtable* socket_desc,u32 data_size)
{
	t_hashtable* cloned_socket_desc = NULL;
	t_socket* socket = NULL;
	t_socket* cloned_socket = NULL;
	u32 i = 0;
	struct t_process_context* current_process_context = NULL;

	CURRENT_PROCESS_CONTEXT(current_process_context);
	cloned_socket_desc = hashtable_init(socket_desc->size);
	for (i=0;i <= current_process_context->next_sd;i++)
	{
		socket = hashtable_get(socket_desc,i);
		if (socket != NULL)
		{
			cloned_socket = kmalloc(sizeof(t_socket));
			kmemcpy(cloned_socket,socket,sizeof(t_socket));
			hashtable_put(cloned_socket_desc,i,cloned_socket);
			if (socket->tcp_conn_desc != NULL) 
			{
				socket->tcp_conn_desc->ref_count++;
				printk("ref count is %d \n",socket->tcp_conn_desc->ref_count);
				printk("i is %d \n",i);
			}
		}
	}
	return cloned_socket_desc;
}

