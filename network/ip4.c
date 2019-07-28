#include "common.h"
#include "network/ip4.h"

static u16 ipv4_id=0;
extern t_hashtable* arp_cache;

int send_packet_ip4(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len,u8 protocol)
{
	//struct t_process_context* current_process_context;
	u16 packet_len;
	char* ip_row_packet;
	u16 chksum_val;
	t_mac_addr* dst_mac;
	t_mac_addr src_mac;
	u8 status;

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
		ip_row_packet[0]= 64+5;                         //VERSION(4) IHL(4)
		ip_row_packet[1]=0;                             //DSFIELD(6) ECN(2)
		ip_row_packet[2]=HI_16(packet_len);             //HIGH TOTAL LENGHT(8)
		ip_row_packet[3]=LOW_16(packet_len);            //LOW TOTAL LENGHT(8)
	
		ip_row_packet[4]=HI_16(ipv4_id++);              //HIGH PACKET ID(8)     
		ip_row_packet[5]=LOW_16(ipv4_id++);             //LOW PACKET ID(8)	
		ip_row_packet[6]=0;                             //HIGH FLAG AND FRAG OFFSET
		ip_row_packet[7]=0;                             //LOW FLAG AND FRAG OFFSET
	
		ip_row_packet[8]=64;                            //TTL(8)
		ip_row_packet[9]=protocol;                      //PROTOCOL(8)
		ip_row_packet[10]=0;                            //HIGH HEADER CRC(8)
		ip_row_packet[11]=0;                            //LOW HEADER CRC(8)
	
		ip_row_packet[12]=HI_OCT_32(src_ip);            //HIGH SRC IP(8)
		ip_row_packet[13]=MID_LFT_OCT_32(src_ip);       //MID LEFT SRC IP(8)
		ip_row_packet[14]=MID_RGT_OCT_32(src_ip);       //MID RIGHY SRC IP(8)
		ip_row_packet[15]=LOW_OCT_32(src_ip);           //LOW SRC IP(8)

		ip_row_packet[16]=HI_OCT_32(dst_ip);            //HI DST IP(8)
		ip_row_packet[17]=MID_LFT_OCT_32(dst_ip);       //MID LEFT DST IP(8)
		ip_row_packet[18]=MID_RGT_OCT_32(dst_ip);       //MID RIGHT IP(8)
		ip_row_packet[19]=LOW_OCT_32(dst_ip);           //LOW DST IP(8)

		chksum_val=SWAP_WORD(checksum((unsigned short*)ip_row_packet,HEADER_IP4));
		ip_row_packet[10]=HI_16(chksum_val);
		ip_row_packet[11]=LOW_16(chksum_val);

		if ((dst_ip & LOCAL_NETMASK) != (system.network_desc->ip & LOCAL_NETMASK))
		{
			dst_mac=lookup_mac(system.network_desc->default_gw_ip);
		}
		else
		{
			dst_mac=lookup_mac(dst_ip);
		}
		
		if(dst_mac==NULL)
                {	
			return 0;
		}
		put_packet_mac(data_sckt_buf,system.network_desc->dev->mac_addr,*dst_mac);
		
	}
	else
	{
		//make here fragmentation
	}
}

void rcv_packet_ip4(t_data_sckt_buf* data_sckt_buf,t_mac_addr* src_mac)
{
	unsigned char* ip_row_packet;
	u32 src_ip;
        u32 dst_ip;
	u16 packet_len;
	t_mac_addr* mac_addr = NULL;
	static int cache_count=0;

	data_sckt_buf->transport_hdr=data_sckt_buf->network_hdr+HEADER_IP4;
	ip_row_packet=data_sckt_buf->network_hdr;
	dst_ip=ip_row_packet[19]+((u32)ip_row_packet[18]<<8)+((u32)ip_row_packet[17]<<16)+((u32)ip_row_packet[16]<<24);
	
	if ((checksum(ip_row_packet,HEADER_IP4)==0) && dst_ip==LOCAL_IP)
	{
		src_ip = ip_row_packet[15]+((u32)ip_row_packet[14]<<8)+((u32)ip_row_packet[13]<<16)+((u32)ip_row_packet[12]<<24);	
		mac_addr = hashtable_get(arp_cache,src_ip);
		if (mac_addr == NULL)
		{
			hashtable_put(arp_cache,src_ip,src_mac);
			cache_count++;
		}
		else
		{
			kfree(src_mac);
		}
		if(ip_row_packet[9] == TCP_PROTOCOL)
		{
			//TCP
			packet_len = ip_row_packet[3] + ((u16) ip_row_packet[2] << 8)- HEADER_IP4 - HEADER_TCP;
			data_sckt_buf->transport_hdr = data_sckt_buf->network_hdr + HEADER_IP4;
			rcv_packet_tcp(data_sckt_buf,src_ip,dst_ip,packet_len);
		}
		else if(ip_row_packet[9] == UDP_PROTOCOL)
		{
			packet_len = ip_row_packet[3] + ((u16) ip_row_packet[2] << 8) - HEADER_IP4 - HEADER_UDP;
			data_sckt_buf->transport_hdr = data_sckt_buf->network_hdr + HEADER_IP4;
			rcv_packet_udp(data_sckt_buf,src_ip,dst_ip,packet_len);
		}
		else if(ip_row_packet[9] == ICMP_PROTOCOL)
		{
			packet_len = ip_row_packet[3] + ((u16) ip_row_packet[2] << 8) - HEADER_IP4 - HEADER_ICMP;
			data_sckt_buf->transport_hdr = data_sckt_buf->network_hdr + HEADER_IP4;
			rcv_packet_icmp(data_sckt_buf,src_ip,dst_ip,packet_len);
			free_sckt(data_sckt_buf);
		}
		else
		{
			printk("unknow protocol \n");
		}
	}
	else 
	{
		//printk("bad checksum \n");
		free_sckt(data_sckt_buf);
		kfree(src_mac);
	}
}
