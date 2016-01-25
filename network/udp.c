#include "network/udp.h"

static u16 checksum_udp(unsigned short* udp_row_packet,u16 data_len);

int send_packet_udp(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port,u16 data_len)
{
	u16 chk;
	char* udp_row_packet;
	u16 ip_packet_len;

	udp_row_packet=data_sckt_buf->transport_hdr;
	data_sckt_buf->network_hdr=data_sckt_buf->transport_hdr-HEADER_UDP;
	ip_packet_len=HEADER_UDP+data_len;

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

		chk=checksum_udp((unsigned short*) udp_row_packet,data_len);
		udp_row_packet[6]=HI_16(chk);
		udp_row_packet[7]=LOW_16(chk);
		send_packet_ip4(data_sckt_buf,src_ip,dst_ip,ip_packet_len,UDP_PROTOCOL);
	}
	else
	{
		//size not supported
	}
}

static u16 checksum_udp(unsigned short* udp_row_packet,u16 data_len)
{
	u16 packet_len;
	u16 chk;

	packet_len=8+HEADER_UDP+data_len;	
	if (packet_len & 1)
	{
		//alloc_sckt has to care of allocating even packet.
		//virtual pad if present has to be discarded using packet len info.
		packet_len++;
	}
	chk=checksum(udp_row_packet-8,packet_len);
	return chk;
}