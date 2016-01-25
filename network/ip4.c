#include "network/ip4.h"

static u16 ipv4_id=0;

/*
static u16 checksum(u8* addr,u32 count)
{
 	register u32 sum = 0;

 	while(count > 1)
  	{
    		sum = sum + (*((u16*) addr))++;
    		count = count - 2;
  	}

 	if (count > 0)
	{
		sum = sum + *((u16*) addr);
	}

  	while (sum>>16)
	{
    		sum = (sum & 0xFFFF) + (sum >> 16);
	}
  	return(~sum);
}
*/

int send_packet_ip4(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len,u8 protocol)
{
	u16 packet_len;
	char* ip_row_packet;
	u16 chksum_val;
	t_mac_addr dst_mac;

	if (data_len+HEADER_IP4>MTU_IP4)
	{
		return -1;
	}
	packet_len=data_len+HEADER_IP4;
	
	if (packet_len<=MTU_ETH)
	{
		ip_row_packet=data_sckt_buf->network_hdr;
		data_sckt_buf->mac_hdr=data_sckt_buf->network_hdr-HEADER_ETH;
	
		//PACKET TAKES BIG ENDIAN/NETWORK BYTE ORDER
		ip_row_packet[0]= 64+5;  			//VERSION(4) IHL(4)
		ip_row_packet[1]=0;	 			//DSFIELD(6) ECN(2)
		ip_row_packet[2]=HI_16(packet_len);     	//HIGH TOTAL LENGHT(8)
		ip_row_packet[3]=LOW_16(packet_len);      	//LOW TOTAL LENGHT(8)
	
		ip_row_packet[4]=HI_16(ipv4_id++);	    	//HIGH PACKET ID(8)     
		ip_row_packet[5]=LOW_16(ipv4_id++);             //LOW PACKET ID(8)	
		ip_row_packet[6]=0;				//HIGH FLAG AND FRAG OFFSET
		ip_row_packet[7]=0;				//LOW FLAG AND FRAG OFFSET
	
		ip_row_packet[8]=64;				//TTL(8)
		ip_row_packet[9]=protocol;   			//PROTOCOL(8)
		ip_row_packet[10]=0;             		//HIGH HEADER CRC(8)
		ip_row_packet[11]=0;             		//LOW HEADER CRC(8)
	
		ip_row_packet[12]=IP_HI_OCT(src_ip);          	//HIGH SRC IP(8)
		ip_row_packet[13]=IP_MID_LFT_OCT(src_ip);	//MID LEFT SRC IP(8)
		ip_row_packet[14]=IP_MID_RGT_OCT(src_ip); 	//MID RIGHY SRC IP(8)
		ip_row_packet[15]=IP_LOW_OCT(src_ip);		//LOW SRC IP(8)

		ip_row_packet[16]=IP_HI_OCT(dst_ip);            //HI DST IP(8)
		ip_row_packet[17]=IP_MID_LFT_OCT(dst_ip);       //MID LEFT DST IP(8)
		ip_row_packet[18]=IP_MID_RGT_OCT(dst_ip);       //MID RIGHT IP(8)
		ip_row_packet[19]=IP_LOW_OCT(dst_ip);           //LOW DST IP(8)

		chksum_val=checksum((unsigned short*)ip_row_packet,HEADER_IP4);
		ip_row_packet[10]=HI_16(chksum_val);
		ip_row_packet[11]=LOW_16(chksum_val);
                 
		u16 chksum_val1=checksum((unsigned short*)ip_row_packet,HEADER_IP4);
		
		dst_mac=lookup_mac(dst_ip);
		put_packet_mac(data_sckt_buf,system.network_desc->dev->mac_addr,dst_mac);
	}
	else
	{
		//make here fragmentation
	}
}

void rcv_packet_ip4(t_data_sckt_buf* data_sckt_buf)
{
	char* ip_row_packet;
	u16 chksum_val;
	u32 src_ip;

	ip_row_packet=data_sckt_buf->network_hdr;
	chksum_val=ip_row_packet[10]+(ip_row_packet[11]<<8);
	src_ip=ip_row_packet[12]+(ip_row_packet[13]<<8)+(ip_row_packet[14]<<8)+(ip_row_packet[15]<<24);
	if ((checksum(ip_row_packet,HEADER_IP4)+chksum_val==0) && src_ip==LOCAL_IP)
	{
		if(ip_row_packet[9]==TCP_PROTOCOL)
		{
			//TCP
		}
		else if(ip_row_packet[9]==UDP_PROTOCOL)
		{
			data_sckt_buf->transport_hdr=data_sckt_buf->transport_hdr-HEADER_UDP;
			//rcv_packet_udp(data_sckt_buf);
		}
		else if(ip_row_packet[9]==ICMP_PROTOCOL)
		{

		}
	}
}
