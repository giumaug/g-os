#include "network/udp.h"

static u16 checksum_udp(char* udp_row_packet,u32 src_ip,u32 dst_ip,u16 data_len);

int send_packet_udp(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port,u16 data_len)
{
	int ret;
	u16 chk;
	unsigned char* udp_row_packet;
	u16 ip_packet_len;

	udp_row_packet=data_sckt_buf->transport_hdr;
	data_sckt_buf->network_hdr=data_sckt_buf->transport_hdr-HEADER_IP4;
	ip_packet_len=HEADER_UDP+data_len;

	src_port=55492;

	if (data_len<=0xFFFF-HEADER_UDP)
	{
		udp_row_packet[0]=HI_16(src_port); 		 //HIGH UDP SOURCE PORT NUMBER
		udp_row_packet[1]=LOW_16(src_port);    		 //LOW UDP SOURCE PORT NUMBER
		udp_row_packet[2]=HI_16(dst_port);		 //HIGH UDP DESTINATION PORT NUMBER
		udp_row_packet[3]=LOW_16(dst_port);    	 	 //LOW UDP DESTINATION PORT NUMBER
 		udp_row_packet[4]=HI_16(HEADER_UDP+data_len);    //HIGH LENGHT
		udp_row_packet[5]=LOW_16(HEADER_UDP+data_len);   //LOW LENGHT
		udp_row_packet[6]=0;	   			 //HIGH CHECKSUM
		udp_row_packet[7]=0;    			 //LOW CHECKSUM

		chk=SWAP_WORD(checksum_udp((unsigned short*) udp_row_packet,src_ip,dst_ip,data_len));
		udp_row_packet[6]=HI_16(chk);
		udp_row_packet[7]=LOW_16(chk);
		ret=send_packet_ip4(data_sckt_buf,src_ip,dst_ip,ip_packet_len,UDP_PROTOCOL);
	}
	else
	{
		//size not supported
	}
	return ret;
}

void rcv_packet_udp(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len)
{
	unsigned char* udp_row_packet;
	t_socket* socket=NULL;
	t_socket_desc* socket_desc=NULL;
	u16 dst_port;

	socket_desc=system.network_desc->socket_desc;
	udp_row_packet=data_sckt_buf->transport_hdr;			 
	dst_port=GET_WORD(udp_row_packet[2],udp_row_packet[3]);

	if (checksum_udp((unsigned short*) udp_row_packet,src_ip,dst_ip,data_len)==0)
	{
		socket=hashtable_get(socket_desc->udp_map,dst_port);
		if (socket!=NULL) 
		{
			if (dst_port==system.network_desc->ip)
			{	
				SPINLOCK_LOCK(socket->lock);
				enqueue(socket->udp_rx_queue,data_sckt_buf);			
				if (socket->process_context!=NULL)
				{
					socket->process_context=NULL;
					_awake(socket->process_context);
				}
				SPINLOCK_UNLOCK(socket->lock);
			}
		}
	}
	else 
	{
		free_sckt(data_sckt_buf);
	}
}

static u16 checksum_udp(char* udp_row_packet,u32 src_ip,u32 dst_ip,u16 data_len)
{
	u16 packet_len;
	u16 chk;
	u16 chk_virt;
	unsigned char header_virt[16];
	unsigned char chk_final[4];
	
	header_virt[0]=IP_HI_OCT(src_ip);          	
	header_virt[1]=IP_MID_LFT_OCT(src_ip);
	header_virt[2]=IP_MID_RGT_OCT(src_ip);
	header_virt[3]=IP_LOW_OCT(src_ip);
	header_virt[4]=IP_HI_OCT(dst_ip);          	
	header_virt[5]=IP_MID_LFT_OCT(dst_ip);
	header_virt[6]=IP_MID_RGT_OCT(dst_ip);
	header_virt[7]=IP_LOW_OCT(dst_ip);		
	header_virt[8]=0;
	header_virt[9]=17;
	header_virt[10]=udp_row_packet[4];
	header_virt[11]=udp_row_packet[5];

	packet_len=HEADER_UDP+data_len;	
	chk=checksum((unsigned short*) udp_row_packet,packet_len);
	chk_virt=checksum((unsigned short*) header_virt,12);

	chk_final[0]=LOW_16(~chk_virt);
	chk_final[1]=HI_16(~chk_virt);
	chk_final[2]=LOW_16(~chk);
	chk_final[3]=HI_16(~chk);
	u16 xxx=checksum(chk_final,4);
	//return ~checksum(chk_final,4);
	return checksum(chk_final,4);
}
