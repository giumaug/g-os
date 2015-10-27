#define IP4_MAX_PACKET_SIZE
#define IP4_FIX_HEADER_SIZE 20
#define MTU_ETH 1526
#define SOCKET_BUFFER_SIZE 2000
#define TCP_PROTOCOL 	4
#define UDP_PROTOCOL 	6
#define ICMP_PROTOCOL	1
#define LOCAL_IP	0xC0A8010A
#define LOCAL_NETMASK   0xFFFFFF00   

#define LOW_16(data) 		(data_len & 0xFF)
#define HI_16(data)  		((data>>8) & 0xFF)
#define IP_LOW_OCT(data)	(data & 0xFF)
#define IP_MID_RGT_OCT(data)	(data>>8 && 0xFF)
#define IP_MID_LFT_OCT(data)	(data>>16 && 0xFF)
#define IP_HI_OCT(data)		(data>>24 && 0xFF)

#define ENQUEUE_PACKET(data_sckt_buf)					\
		SPINLOCK_LOCK(sckt_buf_desc.spinlock);			\
		if (sckt_buf_desc.buf_index+1<=ip4_desc.buf)		\
		{							\
			 enqueue(sckt_buf_desc.buf,data_sckt_buf);	\
			 sckt_buf_desc.buf_index++;			\
		}							\
		SPINLOCK_UNLOCK(sckt_buf_desc.lock);

#define DEQUEUE_PACKET(data_sckt_buf)					\
		SPINLOCK_LOCK(sckt_buf_desc.lock);			\
		if (ip4_desc.buf_index>0)				\
		{							\
			data_sckt_buf=denqueue(sckt_buf_desc.buf);	\
			sckt_buf_desc.buf_index--;			\
		}							\
		SPINLOCK_UNLOCK(sckt_buf_desc.lock);

typedef struct data_sckt_buf
{
	unsigned char* trasport_hdr;
	unsigned char* network_hdr;
	unsigned char* mac_hdr;
	unsigned char* tail;
	unsigned char* end;
	unsigned char* head;
	unsigned char* data;
}
t_data_sckt_buf;

typedef struct sckt_buf_desc
{
	t_queue* buf;
	u32 buf_size;
	u32 buf_index;
	t_spinlock_desc lock;
}
t_sckt_buf_desc;

//DOVE VA MESSO?????????????
static void sckt_buf_desc_init()
{
	sckt_buf_desc.buf=new_queue();
	sckt_buf_desc.buf.buf_size=SOCKET_BUFFER_SIZE;
	sckt_buf_desc.buf.buf_index=0;
	SPINLOCK_INIT(sckt_buf_desc.buf.lock);
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

int put_ip4(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dest_ip,void* data,u16 data_len,u8 protocol)
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
		ip_row_packet=data_sckt_buf->network_hdr;
	
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

//		kmemcpy((ip_row_packet+IP4_FIX_HEADER_SIZE),data,data_len);
//		ENQUEUE_PACKET(packet);
	}
	else
	{
		//make here fragmentation
	}
}

void dequeue_packet_ip4()
{
	char* ip_row_packet;
	t_data_sckt_buf data_sckt_buf=NULL;
	u16 chksum_val;
	u32 src_ip;

	while (DEQUEUE_PACKET(data_sckt_buf))
	{
		ip_row_packet=data_sckt_buf->network_hdr;
		chksum_val=ip_row_packet[10]+(ip_row_packet[11]<<8);
		src_ip=ip_row_packet[12]+(ip_row_packet[13]<<8)+(ip_row_packet[14]<<8)+(ip_row_packet[15]<<24);
		if ((checksum(ip_row_packet,IP4_FIX_HEADER_SIZE)+chksum_val==0) && src_ip==LOCAL_IP)
		{
			if(ip_row_packet[9]==TCP_PROTOCOL)
			{
				//TCP
			}
			else if(ip_row_packet[9]==UDP_PROTOCOL)
			{
				rcv_packet_udp(data_sckt_buf);
			}
			else if(ip_row_packet[9]==ICMP_PROTOCOL)
			{

			}
		}
		kfree(data_sckt_buf);
	}
}
