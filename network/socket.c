#include "network/socket.h"

private int free_port_search(t_hashtable* port_map,u16* port_indx)
{
	u16 src_port_indx;
	void* port=NULL;

	for (i=0;i<32767;i++)
	{
		port_map.udp_indx++;
		src_port_indx=32768+(port_map->udp_map & 0x7FFF);
		port=hashtable_get(socket_desc.udp_map,src_port_indx);
		if (port!=NULL)
		{
			break;
		}
	}
	if (port==NULL)
	{
		src_port=0;	
	}
	return src_port;
}

t_socket_desc* socket_desc_init()
{
	t_socket_desc socket_desc=kmalloc(sizeof(t_socket_desc));
	socket_desc.sd_map=hashtable_init(SOCKET_MAP_SIZE);
	socket_desc.tcp_map=hashtable_init(TCP_MAP_SIZE);
	socket_desc.udp_map=hashtable_init(UDP_MAP_SIZE);
	return socket_desc;
}

void socket_desc_free(t_socket_desc* socket_desc)
{
	hashtable_free(socket_desc.sd_map);
	hashtable_free(socket_desc.tcp_map);
	hashtable_free(socket_desc.udp_map);
	kfree(socket_desc);
	socket_desc.sd_map=NULL;
	socket_desc.tcp_map=NULL;
	socket_desc.udp_map=NULL;

int _open_socket(t_socket_desc* socket_desc,int type) 
{
	t_socket socket=NULL;

	socket=kmalloc(sizeof(t_socket));
	socket->type=type;
	socket_desc->sd++;
	hashtable_put(socket_desc->sd_map,socket_desc->fd,socket);
	if (type==2)
	{
		socket->udp_rx_queue=new new_queue();
		SPINLOCK_INIT(socket->lock);
	}
	return socket_desc->sd;
}

int _bind(t_socket_desc* socket_desc,int sockfd,u32 ip,u32 dst_port)
{
	void* port;
	t_socket socket=NULL;
	u16 src_port=NULL;
	int ret=-1;
	
	socket=hashtable_get(socket_desc->sd_map,sockfd);
	if (socket!=NULL)
	{
		if (hashtable_get(socket_desc.udp_map,dst_port)==NULL)
		{
			socket->src_port=dst_port;
			hashtable_put(socket_desc->udp_map,dst_port,socket);
			ret=0;
		}
	}
	return ret;
}

int _recvfrom(t_socket_desc* socket_desc,int sockfd,u32* src_ip,u16* src_port,void* data) len??????????????????????????????
{
	u32 data_len=0;
	t_socket* socket=NULL;
	t_data_sckt_buf* data_sckt_buf=NULL;

	socket=hashtable_get(socket_desc.udp_map,src_port);
	if (socket!=NULL) 
	{
		SPINLOCK_LOCK(socket->lock);
		data_sckt_buf=dequeue(socket->udp_rx_queue);
		if (data_sckt_buf==NULL)
		{
			CURRENT_PROCESS_CONTEXT(socket->process_context);
			SPINLOCK_UNLOCK(socket->lock);
			_sleep();
		}		 
		*src_port=GET_WORD(data_sckt_buf->transport_hdr[0],data_sckt_buf->transport_hdr[1]);
		*src_ip=GET_DWORD(data_sckt_buf->network_hdr[12],data_sckt_buf->network_hdr[13],data_sckt_buf->network_hdr[14],data_sckt_buf->network_hdr[15]);

		
		kmemcpy(data,data_sckt_buf->transport_hdr+HEADER_UDP,data_len);

		data_sckt_buf->network_hdr;
		src_ip=ip_row_packet[15]+((u32)ip_row_packet[14]<<8)+((u32)ip_row_packet[13]<<16)+((u32)ip_row_packet[12]<<24);
		//kmemcpy(udp_packet->data,udp_row_packet+HEADER_UDP,data_len);
		*src_ip=udp_packet->src_ip;
		*src_port=udp_packet->src_port;
		kmemcpy(ip_payload,data,data_len);
		kmemcpy(data,udp_packet->data,udp_packet->data_len);
		kfree();
		

		u32* src_ip,u16* src_port

		char* data;
	u32 data_len;
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
		
		socket->ip=dst_ip;
		socket->dst_port=dst_port;
		socket->data=data;
		socket->data_len=data_len;
		_sleep();
		socket->data=NULL;
		socket->data_len=0;
		ret=data_len;

		int recvfrom(int sockfd, void* data,u32 data_len,int flags,struct sockaddr* src_addr,socklen_t* addrlen)
	}
	return ret;
}

int _sendto(t_socket_desc* socket_desc,int sockfd,void* data,u32 data_len)
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
			data_sckt_buf=alloc_sckt(33+HEADER_ETH+HEADER_IP4+HEADER_UDP);
			data_sckt_buf->transport_hdr=data_sckt_buf->data+HEADER_ETH+HEADER_IP4;
			ip_payload=data_sckt_buf->transport_hdr+HEADER_UDP;
			kmemcpy(ip_payload,data,data_len);
			ret=send_packet_udp(data_sckt_buf,socket->src_ip,socket->dst_ip,socket->src_port,socket->dst_port,data_len);
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

int _close_socket(t_socket_desc* socket_desc,int int sockfd)
{
	t_socket* socket=NULL;

	socket=hashtable_get(socket_desc->sd_map,sockfd);
	if (socket->type==2)
	{
		hashtable_remove(socket_desc.udp_map,socket);
		free_queue(socket->udp_rx_queue);
	}
	else
	{
		hashtable_remove(socket_desc.tcp_map,socket);
	}
	kfree(socket);
}
