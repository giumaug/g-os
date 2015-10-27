typedef struct s_mac_addr
{
	u16 lo;
	u16 mi;
	u16 hi;
}
t_mac_addr


(t_data_sckt_buf* data_sckt_buf,_mac_addr src_mac,_mac_addr dest_mac)

void put_packet_mac(t_data_sckt_buf* data_sckt_buf,_mac_addr src_mac,_mac_addr dst_mac)
{
	char* mac_row_packet;

	mac_row_packet=data_sckt_buf->mac_hdr;
	
	mac_row_packet[0]=0xAA;  		//PREAMBLE
	mac_row_packet[1]=0xAA;			//PREAMBLE
	mac_row_packet[2]=0xAA;			//PREAMBLE
	mac_row_packet[3]=0xAA;			//PREAMBLE
	mac_row_packet[4]=0xAA;			//PREAMBLE
	mac_row_packet[5]=0xAA;			//PREAMBLE
	mac_row_packet[6]=0xAA;			//PREAMBLE

	mac_row_packet[7]=LOW_16(src_mac.lo); 	//BYTE 1 SRC MAC ADDRESS
	mac_row_packet[8]=HI_16(src_mac.lo);	//BYTE 2 SRC MAC ADDRESS
	mac_row_packet[9]=LOW_16(src_mac.mi); 	//BYTE 3 SRC MAC ADDRESS
	mac_row_packet[10]=HI_16(src_mac.mi); 	//BYTE 4 SRC MAC ADDRESS
	mac_row_packet[11]=LOW_16(src_mac.hi); 	//BYTE 5 SRC MAC ADDRESS
	mac_row_packet[12]=HI_16(src_mac.hi); 	//BYTE 6 SRC MAC ADDRESS

	mac_row_packet[13]=LOW_16(dst_mac.lo);	//BYTE 1 DST MAC ADDRESS
	mac_row_packet[14]=HI_16(dst_mac.lo);	//BYTE 2 DST MAC ADDRESS
	mac_row_packet[15]=LOW_16(dst_mac.mi); 	//BYTE 3 DST MAC ADDRESS
	mac_row_packet[16]=HI_16(dst_mac.mi); 	//BYTE 4 DST MAC ADDRESS
	mac_row_packet[17]=LOW_16(dst_mac.hi); 	//BYTE 5 DST MAC ADDRESS
	mac_row_packet[18]=HI_16(dst_mac.hi); 	//BYTE 6 DST MAC ADDRESS

	

	

	


}

void dequeue_packet_mac()
{

}
