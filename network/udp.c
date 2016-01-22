include "network/udp.h"

int send_packet_ip4(t_data_sckt_buf* data_sckt_buf,u16 src_port,u16 dst_port,u16 data_len)
{
	char* udp_row_packet;

	udp_row_packet=data_sckt_buf->transport_hdr;
	data_sckt_buf->network_hdr=data_sckt_buf->transport_hdr-HEADER_UDP;

	if (packet_len<=0xFFFF-HEADER_UDP)
	{
		udp_row_packet[0]=LOW_16(src_port); 		//LOW UDP SOURCE PORT NUMBER
		udp_row_packet[1]=HI_16(src_port);    		//HIGH UDP SOURCE PORT NUMBER
		udp_row_packet[2]=LOW_16(dst_port);		//LOW UDP DESTINATION PORT NUMBER
		udp_row_packet[3]=HI_16(dst_port);    		//HIGH UDP DESTINATION PORT NUMBER
 		udp_row_packet[4]=LOW_16(HEADER_UDP+data_len);  //LOW LENGHT
		udp_row_packet[5]=HI_16(HEADER_UDP+data_len);   //HIGH LENGHT
		udp_row_packet[6]=LOW_16();   			//LOW CHECKSUM
		udp_row_packet[7]=HI_16();    			//HIGH CHECKSUM
	}
	else
	{
		//size not supported
	}
}
