#include "network/mac.h"

void put_packet_mac(t_data_sckt_buf* data_sckt_buf,t_mac_addr src_mac,t_mac_addr dst_mac)
{
	char* mac_row_packet;

	mac_row_packet=data_sckt_buf->mac_hdr;
	
//	mac_row_packet[0]=0xAA;  				//PREAMBLE
//	mac_row_packet[1]=0xAA;					//PREAMBLE
//	mac_row_packet[2]=0xAA;					//PREAMBLE
//	mac_row_packet[3]=0xAA;					//PREAMBLE
//	mac_row_packet[4]=0xAA;					//PREAMBLE
//	mac_row_packet[5]=0xAA;					//PREAMBLE
//	mac_row_packet[6]=0xAA;					//PREAMBLE

	mac_row_packet[0]=LOW_16(src_mac.lo); 			//BYTE 1 SRC MAC ADDRESS
	mac_row_packet[1]=HI_16(src_mac.lo);			//BYTE 2 SRC MAC ADDRESS
	mac_row_packet[2]=LOW_16(src_mac.mi); 			//BYTE 3 SRC MAC ADDRESS
	mac_row_packet[3]=HI_16(src_mac.mi); 			//BYTE 4 SRC MAC ADDRESS
	mac_row_packet[4]=LOW_16(src_mac.hi); 			//BYTE 5 SRC MAC ADDRESS
	mac_row_packet[5]=HI_16(src_mac.hi); 			//BYTE 6 SRC MAC ADDRESS

	mac_row_packet[6]=LOW_16(dst_mac.lo);			//BYTE 1 DST MAC ADDRESS
	mac_row_packet[7]=HI_16(dst_mac.lo);			//BYTE 2 DST MAC ADDRESS
	mac_row_packet[8]=LOW_16(dst_mac.mi); 			//BYTE 3 DST MAC ADDRESS
	mac_row_packet[9]=HI_16(dst_mac.mi); 			//BYTE 4 DST MAC ADDRESS
	mac_row_packet[10]=LOW_16(dst_mac.hi); 			//BYTE 5 DST MAC ADDRESS
	mac_row_packet[11]=HI_16(dst_mac.hi); 		   	//BYTE 6 DST MAC ADDRESS

	mac_row_packet[12]=LOW_16(NETWORK_PROTOCOL_TYPE);  	//LOW PROTOCOL TYPE
	mac_row_packet[13]=HI_16(NETWORK_PROTOCOL_TYPE);		//PROTOCOL TYPE
}

void rcv_packet_mac(t_data_sckt_buf* data_sckt_buf)
{
	data_sckt_buf->mac_hdr=data_sckt_buf->mac_hdr-HEADER_ETH;
	rcv_packet_ip4(data_sckt_buf);
}
