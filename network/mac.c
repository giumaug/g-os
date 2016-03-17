#include "network/mac.h"

void put_packet_mac(t_data_sckt_buf* data_sckt_buf,t_mac_addr src_mac,t_mac_addr dst_mac)
{
	char* mac_row_packet;

//      dst_mac.lo=0x8;
//	dst_mac.mi=0x56c0;
//       dst_mac.hi=0x50;

//      src_mac.lo=0x64b9;
//	src_mac.mi=0x2910;
//      src_mac.hi=0xc;

	mac_row_packet=data_sckt_buf->mac_hdr;
	
	mac_row_packet[0]=HI_16(dst_mac.hi); 			//BYTE 1 DST MAC ADDRESS
	mac_row_packet[1]=LOW_16(dst_mac.hi);			//BYTE 2 DST MAC ADDRESS
	mac_row_packet[2]=HI_16(dst_mac.mi); 			//BYTE 3 DST MAC ADDRESS
	mac_row_packet[3]=LOW_16(dst_mac.mi); 			//BYTE 4 DST MAC ADDRESS
	mac_row_packet[4]=HI_16(dst_mac.lo); 			//BYTE 5 DST MAC ADDRESS
	mac_row_packet[5]=LOW_16(dst_mac.lo); 			//BYTE 6 DST MAC ADDRESS

	mac_row_packet[6]=HI_16(src_mac.hi);			//BYTE 1 SRC MAC ADDRESS
	mac_row_packet[7]=LOW_16(src_mac.hi);			//BYTE 2 SRC MAC ADDRESS
	mac_row_packet[8]=HI_16(src_mac.mi); 			//BYTE 3 SRC MAC ADDRESS
	mac_row_packet[9]=LOW_16(src_mac.mi); 			//BYTE 4 SRC MAC ADDRESS
	mac_row_packet[10]=HI_16(src_mac.lo); 			//BYTE 5 SRC MAC ADDRESS
	mac_row_packet[11]=LOW_16(src_mac.lo); 		   	//BYTE 6 SRC MAC ADDRESS

	mac_row_packet[12]=HI_16(NETWORK_PROTOCOL_TYPE);  	//HIGH PROTOCOL TYPE
	mac_row_packet[13]=LOW_16(NETWORK_PROTOCOL_TYPE);	//LOW PROTOCOL TYPE

	enqueue_sckt(system.network_desc->tx_queue,data_sckt_buf);
}

void rcv_packet_mac(t_data_sckt_buf* data_sckt_buf)
{
	u16 protocol_type;

	data_sckt_buf->network_hdr=data_sckt_buf->mac_hdr+HEADER_ETH;
	
	protocol_type=GET_WORD(data_sckt_buf->mac_hdr[12],data_sckt_buf->mac_hdr[13]);
	if (protocol_type==ARP_PROTOCOL_TYPE)
	{
		rcv_packet_arp(data_sckt_buf);
	}
	else
	{ 
		rcv_packet_ip4(data_sckt_buf);
	}
}
