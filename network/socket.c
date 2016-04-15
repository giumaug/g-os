#include "network/socket.h"

static int free_port_search()
{
	int i;
	u16 src_port_indx=0;
	u16 port_indx=0;
	void* port=NULL;
	t_socket_desc* socket_desc=NULL;

	socket_desc=system.network_desc->socket_desc;
	src_port_indx=socket_desc->udp_port_indx;
	for (i=0;i<32767;i++)
	{
		port_indx++;
		src_port_indx=(32768+port_indx & 0x7FFF);
		port=hashtable_get(socket_desc->udp_map,port_indx);
		if (port==NULL)
		{
			socket_desc->udp_port_indx=src_port_indx;
			return src_port_indx;
		}
	}
	return 0;
}

t_socket_desc* socket_desc_init()
{
	t_socket_desc* socket_desc=kmalloc(sizeof(t_socket_desc));
	socket_desc->sd_map=hashtable_init(SOCKET_MAP_SIZE);
	socket_desc->tcp_map=hashtable_init(TCP_MAP_SIZE);
	socket_desc->udp_map=hashtable_init(UDP_MAP_SIZE);
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

int _open_socket(t_socket_desc* socket_desc,int type) 
{
	t_socket* socket=NULL;

	socket=kmalloc(sizeof(t_socket));
	socket->type=type;
	socket->sd++;
	hashtable_put(socket_desc->sd_map,socket_desc->fd,socket);
	if (type==2)
	{
		socket->udp_rx_queue=new_queue();
		socket->lock=kmalloc(sizeof(t_spinlock_desc));
		SPINLOCK_INIT(*socket->lock);
	}
	return socket->sd;
}

int _bind(t_socket_desc* socket_desc,int sockfd,u32 ip,u32 src_port)
{
	void* port;
	t_socket* socket=NULL;
	int ret=-1;
	
	socket=hashtable_get(socket_desc->sd_map,sockfd);
	if (socket!=NULL)
	{
		if (hashtable_get(socket_desc->udp_map,src_port)==NULL)
		{
			socket->port=src_port;
			hashtable_put(socket_desc->udp_map,src_port,socket);
			ret=0;
		}
	}
	return ret;
}

int _recvfrom(t_socket_desc* socket_desc,int sockfd,u32* src_ip,u16* src_port,void* data,u32 data_len)
{
	u32 read_data=0;
	t_socket* socket=NULL;
	t_data_sckt_buf* data_sckt_buf=NULL;

	socket=hashtable_get(socket_desc->udp_map,src_port);
	if (socket!=NULL) 
	{
		SPINLOCK_LOCK(*socket->lock);
		data_sckt_buf=dequeue(socket->udp_rx_queue);
		if (data_sckt_buf==NULL)
		{
			_sleep_and_unlock(socket->lock);
		}		 
		*src_port=GET_WORD(data_sckt_buf->transport_hdr[0],data_sckt_buf->transport_hdr[1]);
		*src_ip=GET_DWORD(data_sckt_buf->network_hdr[12],data_sckt_buf->network_hdr[13],data_sckt_buf->network_hdr[14],data_sckt_buf->network_hdr[15]);
		read_data=GET_WORD(data_sckt_buf->transport_hdr[4],data_sckt_buf->transport_hdr[5])-HEADER_UDP;
		if (read_data > data_len) 
		{
			read_data=data_len;
		}
		kmemcpy(data,data_sckt_buf->transport_hdr+HEADER_UDP,read_data);
		kfree(data_sckt_buf);
	}
	return read_data;
}


int _sendto(t_socket_desc* socket_desc,int sockfd,u32 dst_ip,u16 dst_port,void* data,u32 data_len)
{
	t_socket* socket=NULL;
	t_data_sckt_buf* data_sckt_buf=NULL;
	char* ip_payload=NULL;
	int ret=0;
	
	socket=hashtable_get(socket_desc->sd_map,sockfd);
	if (socket!=NULL)
	{
		if (socket->type==2)
		{
			if (socket->port==0) 
			{
				socket->port=free_port_search();
				socket_desc=system.network_desc->socket_desc;
				hashtable_put(socket_desc->udp_map,socket->port,socket);
			}
			data_sckt_buf=alloc_sckt(33+HEADER_ETH+HEADER_IP4+HEADER_UDP);
			data_sckt_buf->transport_hdr=data_sckt_buf->data+HEADER_ETH+HEADER_IP4;
			ip_payload=data_sckt_buf->transport_hdr+HEADER_UDP;
			kmemcpy(ip_payload,data,data_len);
			ret=send_packet_udp(data_sckt_buf,system.network_desc->ip,dst_ip,socket->port,dst_port,data_len);

			if (ret==0)
			{
				ret=data_len;
			}
		}
	}
	if (ret>0)
	{
		kfree(socket);
	}
	return ret;
}

int _close_socket(t_socket_desc* socket_desc,int sockfd)
{
	t_socket* socket=NULL;

	socket=hashtable_get(socket_desc->sd_map,sockfd);
	if (socket->type==2)
	{
		hashtable_remove(socket_desc->udp_map,socket);
		free_queue(socket->udp_rx_queue);
		kfree(socket->lock);
	}
	else
	{
		hashtable_remove(socket_desc->tcp_map,socket);
	}
	kfree(socket);
}

