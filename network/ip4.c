
#define IP4_MAX_PACKET_SIZE
#define IP4_FIX_HEADER_SIZE 160
#define MTU_ETH 1526

typedef struct s_ip4_header
{
	u8 ver;
	u8 ihl;
	u8 dsf;
	u16 tot_len;
	u16 id;
	u8 flags:
	u16 frag_offset;
	u8 ttl;
	u8 protocol;
	u16 header_chk;
	u32 src_addr;
	u32 dst_addr;
	char* opt;
}
t_ip4_header;

typedef struct s_ip4_desc
{
	t_queue* ip_buffer;
	t_arp* arp_desc;
}



int ip_put(u32 src_ip,u32 dest_ip,void* data,u16 len)
{
	char* ip_row_packet;

	if (len>IP4_MAX_PACKET_SIZE)
	{
		return -1;
	}

	src_mac=from_ip_to_mac(src_ip)
	if (src_mac.hi==0xff)
	{
		return -1;
	}

	dest_ip=from_mac_to_ip(dest_ip)
	if (dest_mac.hi==0xff)
	{
		return -1;
	}
	
	if (len+IP4_FIX_HEADER_SIZE>MTU_ETH)
	{
		return -1;
	}

	ip_row_packet=kmalloc(IP4_FIX_HEADER_SIZE+len);
	
	ip_row_packet[0]=
	ip_row_packet[1]=
	ip_row_packet[2]=
	ip_row_packet[3]=
	ip_row_packet[4]=
	ip_row_packet[5]=
	ip_row_packet[6]=

	ip_row_packet[7]=
	ip_row_packet[8]=
	ip_row_packet[9]=
	ip_row_packet[10]=
	ip_row_packet[11]=
	ip_row_packet[12]=
	ip_row_packet[13]=

	ip_row_packet[14]=
	ip_row_packet[15]=
	ip_row_packet[16]=
	ip_row_packet[17]=
	ip_row_packet[18]=
	ip_row_packet[19]=
	ip_row_packet[20]=




}

ip_get()
{

}
