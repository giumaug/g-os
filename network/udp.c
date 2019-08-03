#include "common.h"
#include "network/udp.h"

static u16 checksum_udp(char* udp_row_packet,u32 src_ip,u32 dst_ip,u16 data_len);

int udp_free_port_search()
{
	int i;
	void* port = NULL;
	t_udp_desc* udp_desc = NULL;

	udp_desc = system.network_desc->udp_desc;
	for (i = 0;i < 32767;i++)
	{
		udp_desc->ep_port_index++;
		if (udp_desc->ep_port_index > 65535) 
		{
			udp_desc->ep_port_index = 32767;
		}
		port = hashtable_get(udp_desc->ep_port_map,udp_desc->ep_port_index);
		if (port == NULL)
		{
			hashtable_put(udp_desc->ep_port_map,udp_desc->ep_port_index,1);
			return udp_desc->ep_port_index;
		}
	}
	return 0;
}

t_udp_desc* udp_init()
{
	t_udp_desc* udp_desc = NULL;

	udp_desc = kmalloc(sizeof(t_udp_desc));
	udp_desc->ep_port_map = hashtable_init(UDP_EPMRL_PORT_MAP_SIZE);
	udp_desc->conn_map = hashtable_init(UDP_CONN_MAP_SIZE);
	udp_desc->ep_port_index = 32767;
	SPINLOCK_INIT(udp_desc->lock);
	return udp_desc;
}

void udp_free(t_udp_desc* udp_desc)
{
	hashtable_free(udp_desc->ep_port_map);
	kfree(udp_desc);
}

t_udp_conn_desc* udp_conn_desc_init()
{
	t_udp_conn_desc* udp_conn_desc = NULL;
	udp_conn_desc = kmalloc(sizeof(t_udp_conn_desc));
        udp_conn_desc->rx_queue = dc_new_queue(&free_sckt);
	return udp_conn_desc;
}

void udp_conn_desc_free(t_udp_conn_desc* udp_conn_desc)
{
	hashtable_remove(system.network_desc->udp_desc->ep_port_map,udp_conn_desc->src_port);
	free_queue(udp_conn_desc->rx_queue);
	kfree(udp_conn_desc);
}

bind_udp(t_udp_conn_desc* udp_conn_desc,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port)
{
	SAVE_IF_STATUS
	CLI
	udp_conn_desc->src_ip = src_ip;
	udp_conn_desc->src_port = src_port;
	udp_conn_desc->dst_ip = 0;
	udp_conn_desc->dst_port = 0;
	RESTORE_IF_STATUS
	return 0;
}

int send_packet_udp(u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port,char* data,u16 data_len)
{
	int ret;
	u16 chk;
	unsigned char* udp_row_packet;
	u16 ip_packet_len;
	t_data_sckt_buf* data_sckt_buf = NULL;
	char* ip_payload = NULL;

	SAVE_IF_STATUS
	CLI
	ret = 0;
	data_sckt_buf = alloc_sckt(data_len + HEADER_ETH + HEADER_IP4 + HEADER_UDP);
	data_sckt_buf->transport_hdr = data_sckt_buf->data + HEADER_ETH + HEADER_IP4;
	ip_payload = data_sckt_buf->transport_hdr + HEADER_UDP;
	kmemcpy(ip_payload,data,data_len);
	udp_row_packet = data_sckt_buf->transport_hdr;
	data_sckt_buf->network_hdr = data_sckt_buf->transport_hdr - HEADER_IP4;
	ip_packet_len = HEADER_UDP + data_len;

	if (data_len<=0xFFFF-HEADER_UDP)
	{
		udp_row_packet[0] = HI_16(src_port);               //HIGH UDP SOURCE PORT NUMBER
		udp_row_packet[1] = LOW_16(src_port);              //LOW UDP SOURCE PORT NUMBER
		udp_row_packet[2] = HI_16(dst_port);               //HIGH UDP DESTINATION PORT NUMBER
		udp_row_packet[3] = LOW_16(dst_port);              //LOW UDP DESTINATION PORT NUMBER
 		udp_row_packet[4] = HI_16(HEADER_UDP+data_len);    //HIGH LENGHT
		udp_row_packet[5] = LOW_16(HEADER_UDP+data_len);   //LOW LENGHT
		udp_row_packet[6] = 0;                             //HIGH CHECKSUM
		udp_row_packet[7] = 0;                             //LOW CHECKSUM

		chk=SWAP_WORD(checksum_udp((unsigned short*) udp_row_packet,src_ip,dst_ip,data_len));
		udp_row_packet[6]=HI_16(chk);
		udp_row_packet[7]=LOW_16(chk);
		send_packet_ip4(data_sckt_buf,src_ip,dst_ip,ip_packet_len,UDP_PROTOCOL);
		ret = data_len;
	}
	else
	{
		//size not supported
	}
	RESTORE_IF_STATUS
	return ret;
}

void rcv_packet_udp(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len)
{
	static int xxx = 0;
	unsigned char* udp_row_packet;
	t_socket* socket = NULL;
	u16 dst_port;

	SAVE_IF_STATUS
	CLI
	udp_row_packet=data_sckt_buf->transport_hdr;			 
	dst_port=GET_WORD(udp_row_packet[2],udp_row_packet[3]);

	if (checksum_udp((unsigned short*) udp_row_packet,src_ip,dst_ip,data_len)==0)
	{
		socket = hashtable_get(system.network_desc->udp_desc->conn_map,dst_port);
		if (socket!=NULL) 
		{
			enqueue(socket->udp_conn_desc->rx_queue,data_sckt_buf);			
			if (socket->process_context != NULL)
			{
				_awake(socket->process_context);
				socket->process_context = NULL;
			}
		}
		else
		{
			free_sckt(data_sckt_buf);
		}
	}
	else 
	{
		free_sckt(data_sckt_buf);
	}
	RESTORE_IF_STATUS
}

void close_udp(t_socket* socket)
{
	SAVE_IF_STATUS
	CLI
	hashtable_remove(system.network_desc->udp_desc->ep_port_map,socket->udp_conn_desc->src_ip);
	hashtable_remove(system.network_desc->udp_desc->conn_map,socket->udp_conn_desc->src_port);
	udp_conn_desc_free(socket->udp_conn_desc);
	RESTORE_IF_STATUS
}

int dequeue_packet_udp(int sockfd,unsigned char* src_ip,unsigned char* src_port,void* data,u32 data_len)
{
	struct t_process_context* process_context = NULL;
	t_socket* socket = NULL;
	t_data_sckt_buf* data_sckt_buf = NULL;
	unsigned int _src_ip;
	unsigned int _src_port;
	u32 read_data;

	SAVE_IF_STATUS
	CLI
	CURRENT_PROCESS_CONTEXT(process_context);
	socket = hashtable_get(process_context->socket_desc,sockfd);
	data_sckt_buf = dequeue(socket->udp_conn_desc->rx_queue);
	if (data_sckt_buf == NULL)
	{
		socket->process_context = process_context;
		_sleep();
		data_sckt_buf = dequeue(socket->udp_conn_desc->rx_queue);
		if (data_sckt_buf == NULL) 
		{
			printk("error on udp socket \n");
			return -1;
		}	
	} 
	_src_port = GET_WORD(data_sckt_buf->transport_hdr[0],data_sckt_buf->transport_hdr[1]);
	_src_ip = GET_DWORD(data_sckt_buf->network_hdr[12],
			    data_sckt_buf->network_hdr[13],
                            data_sckt_buf->network_hdr[14],
                            data_sckt_buf->network_hdr[15]);
	read_data = GET_WORD(data_sckt_buf->transport_hdr[4],data_sckt_buf->transport_hdr[5])-HEADER_UDP;

	src_ip[0] = (_src_ip & 0xFF000000)>>24;
	src_ip[1] = (_src_ip & 0xFF0000)>>16;
	src_ip[2] = (_src_ip & 0xFF00)>>8;
	src_ip[3] = (_src_ip & 0xFF);

	src_port[0] = (_src_port & 0xFF00)>>8;
	src_port[1] = (_src_port & 0xFF);

	if (read_data > data_len) 
	{
		read_data = data_len;
	}
	kmemcpy(data,data_sckt_buf->transport_hdr+HEADER_UDP,read_data);
	free_sckt(data_sckt_buf);
	RESTORE_IF_STATUS
	return read_data;
}

static u16 checksum_udp(char* udp_row_packet,u32 src_ip,u32 dst_ip,u16 data_len)
{
	u16 packet_len;
	u16 chk;
	u16 chk_virt;
	unsigned char header_virt[16];
	unsigned char chk_final[4];
	
	header_virt[0] = HI_OCT_32(src_ip);          	
	header_virt[1] = MID_LFT_OCT_32(src_ip);
	header_virt[2] = MID_RGT_OCT_32(src_ip);
	header_virt[3] = LOW_OCT_32(src_ip);
	header_virt[4] = HI_OCT_32(dst_ip);          	
	header_virt[5] = MID_LFT_OCT_32(dst_ip);
	header_virt[6] = MID_RGT_OCT_32(dst_ip);
	header_virt[7] = LOW_OCT_32(dst_ip);		
	header_virt[8] = 0;
	header_virt[9] = UDP_PROTOCOL;
	header_virt[10] = udp_row_packet[4];
	header_virt[11] = udp_row_packet[5];

	packet_len = HEADER_UDP + data_len;	
	chk = checksum((unsigned short*) udp_row_packet,packet_len);
	chk_virt = checksum((unsigned short*) header_virt,12);

	chk_final[0]=LOW_16(~chk_virt);
	chk_final[1]=HI_16(~chk_virt);
	chk_final[2]=LOW_16(~chk);
	chk_final[3]=HI_16(~chk);
	return checksum(chk_final,4);
}
