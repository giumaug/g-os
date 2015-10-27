
#define IP4_MAX_PACKET_SIZE
#define IP4_FIX_HEADER_SIZE 20
#define MTU_ETH 1526
#define IP4_BUFFER_SIZE 2000

#define LOW_16(data) 		(data_len & 0xFF)
#define HI_16(data)  		((data>>8) & 0xFF)
#define IP_LOW_OCT(data)	(data & 0xFF)
#define IP_MID_RGT_OCT(data)	(data>>8 && 0xFF)
#define IP_MID_LFT_OCT(data)	(data>>16 && 0xFF)
#define IP_HI_OCT(data)		(data>>24 && 0xFF)

#define ENQUEUE_PACKET(packet)						\
		SPINLOCK_LOCK(ip4_desc.spinlock);			\
		if (ip4_desc.buf_index+1<=ip4_desc.buf)			\
		{							\
			 enqueue(ip4_desc.ip_buf,packet);		\
			 ip4_desc.buf_index++;				\
		}							\
		SPINLOCK_UNLOCK(ip4_desc.spinlock);

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
	t_queue* ip_buf;
	t_arp* arp_desc;
	u16 packet_id;
	u32 buf_size;
	u32 buf_index;
	t_spinlock_desc spinlock;
}
t_ip4_desc ip4_desc;

static t_ip4_desc ip4_desc;

void init_ip4()
{
	ip4_desc.ip_buf=new_queue();
	ip4_desc.packet_id=0;
	ip4_desc.buf_size=IP4_BUFFER_SIZE;
	ip4_desc.buf_index=0;
	SPINLOCK_INIT(ip4_desc.spinlock);
}

int put_ip4(u32 src_ip,u32 dest_ip,void* data,u16 data_len,u8 protocol)
{
	u16 packet_len;
	char* ip_row_packet;
	u16 chksum_val;

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
	
	packet_len=data_len+IP4_FIX_HEADER_SIZE;
	
	if (packet_len<=MTU_ETH)
	{
		ip_row_packet=kmalloc(IP4_FIX_HEADER_SIZE+len);
	
		//PACKET TAKES BIG ENDIAN/NETWORK BYTE ORDER
		ip_row_packet[0]= (1>>3) | 5; 			//VERSION(4) IHL(4)
		ip_row_packet[1]=0;	 			//DSFIELD(6) ECN(2)
		ip_row_packet[2]=LOW_16(packet_len);     	//LOW TOTAL LENGHT(8)
		ip_row_packet[3]=HI_16(packet_len);      	//HI TOTAL LENGHT(8)
	
		ip_row_packet[4]=LOW_16(ip4_desc.packet_id);    //LOW PACKET ID(8)       
		ip_row_packet[5]=HI_16(ip4_desc.packet_id);     //HI PACKET ID(8)	
		ip_row_packet[6]=0				//LOW FLAG AND FRAG OFFSET
		ip_row_packet[7]=0				//HI FLAG AND FRAG OFFSET
	
		ip_row_packet[8]=64;				//TTL(8)
		ip_row_packet[9]=protocol;   			//PROTOCOL(8)
		ip_row_packet[10]=0;             		//LOW HEADER CRC(8)
		ip_row_packet[11]=0;             		//HI HEADER CRC(8)
	
		ip_row_packet[12]=IP_LOW_OCT(src_ip);          	//LOW SRC IP(8)
		ip_row_packet[13]=IP_MID_LFT_OCT(src_ip);	//MID RIGHT  SRC IP(8)
		ip_row_packet[14]=IP_MID_RGT(src_ip); 		//MID LEFT SRC IP(8)
		ip_row_packet[15]=IP_HI_OCT(src_ip);		//HI SRC IP(8)

		ip_row_packet[16]=IP_LOW_OCT(dest_ip);          //LOW DST IP(8)
		ip_row_packet[17]=IP_MID_LFT_OCT(dest_ip);	//MID RIGHT DST IP(8)
		ip_row_packet[18]=IP_MID_RGT(dest_ip);          //MID LEFT IP(8)
		ip_row_packet[19]=IP_HI_OCT(dest_ip);           //HI DST IP(8)

		chksum_val=checksum(ip_row_packet,IP4_FIX_HEADER_SIZE);
		ip_row_packet[10]=LOW_16(chksum_val);
		ip_row_packet[11]=HI_16(chksum_val);

		kmemcpy((ip_row_packet+IP4_FIX_HEADER_SIZE),data,data_len);
		ENQUEUE_PACKET(packet);
	}
	else
	{
		//make here fragmentation
	}
}

u16 checksum(byte* addr,u32 count)
{
 	register u32 sum = 0;

 	while(count > 1)
  	{
    	sum = sum + *((word16 *) addr)++;
    	count = count - 2;
  	}

 	if (count > 0)
	{
		sum = sum + *((byte *) addr);
	}

  	while (sum>>16)
	{
    		sum = (sum & 0xFFFF) + (sum >> 16);
	}
  	return(~sum);
}

dequeue_packet_ip4()
{
	char* ip_row_packet;

	while (ip4_desc.buf_index!=0)
	{
		ip_row_packet=dequeue(ip4_desc.ip_buf);
	}

}
