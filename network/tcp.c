//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANTE VERIFICARE CHE LA STRUTTURA DATI COMPLESSIVA SIA SENSATA!!!!!!!!!!!!!!!!!!

#define TCP_RCV_SIZE 
#define TCP_SND_SIZE
#define TCP_CONN_MAP_SIZE 	20

#define INC_WND(cur,wnd_size,offset)  (cur + offset) % wnd_size

typedef struct s_packet
{
	void* val;
	u8 status;
}
t_packet;


typedef struct s_tcp_queue
{
	u32 min;
	u32 max;
	u32 cur;
	u32 size;
	t_packet* buf;
}
t_tcp_queue;

typedef struct s_tcp_conn_desc
{
	u64 conn_id;
	t_tcp_queue* rcv_buf;
	t_tcp_queue* snd_buf;
}
t_tcp_conn_desc;

typedef struct s_tcp_desc
{
	t_hashtable* conn_map;
}
t_tcp_desc;

t_tcp_queue tcp_queue_init(u32 size)
{
	int i;
	t_tcp_queue* tcp_queue;

	tcp_queue->buf=kmalloc(sizeof(t_packet*)*size);
	for (i=0;i<size;i++)
	{
		tcp_queue->buf[i]=NULL;
	}
	tcp_queue->min=0;
	tcp_queue->max=0;
	tcp_queue->cur=0;
	tcp_queue->size=size;
	return tcp_queue;
}

void tcp_queue_free(tcp_queue* tcp_queue)
{
	kfree(tcp_queue);
}

t_tcp_conn_desc* tcp_conn_desc_int(u16 rcv_src_port,u16 rcv_dst_port,u16 snd_src_port,u16 snd_dst_port)
{
	t_tcp_conn_desc* tcp_conn_desc;

	tcp_conn_desc=kmalloc(sizeof(t_tcp_conn_desc));
	tcp_conn_desc->rcv_buf=tcp_queue_init(TCP_SND_SIZE);
	tcp_conn_desc->rcv_buf=tcp_queue_init(TCP_RCV_SIZE);
	tcp_conn_desc->conn_id=rcv_src_port | (rcv_src_port<<8) | (snd_src_port<<16) | (snd_src_port<<24));
	return tcp_conn_desc;
}

void tcp_conn_desc_free(t_tcp_conn_desc* tcp_conn_desc)
{
	tcp_queue_free(tcp_conn_desc->rcv_buf);
	tcp_queue_free(tcp_conn_desc->snd_buf);
	kfree(tcp_conn_desc);
}

t_tcp_desc* tcp_init()
{
	t_tcp_desc* tcp_desc;
	
	tcp_desc=kmalloc(sizeof(t_tcp_desc));
	tcp_desc->conn_map=dc_hashtable_init(TCP_CONN_MAP_SIZE,tcp_conn_desc_free);
	return tcp_desc;
}

void tcp_free(t_tcp_desc* tcp_desc)
{
	hashtable_free(tcp_desc->conn_map);
	kfree(tcp_desc);
}

int tcp_queue_add(t_tcp_queue* tcp_queue,t_data_sckt_buf* data_sckt_buf,u8 status)
{
	if (INC_WND(tcp_queue->cur,tcp_queue->size,1))
	{
		return -1;
	}
	tcp_queue->cur=INC_WND(tcp_queue->cur,tcp_queue->size,1);
	
	t_packet packet=kmalloc(sizeof(t_packet));
	packet->status=status;
	packet->val=data_sckt_buf;
	tcp_queue->buf[tcp_queue->cur]=data_sckt_buf;
	return 0;
}



tcp_queue_del(t_tcp_queue* tcp_queue,t_data_sckt_buf* data_sckt_buf)
{
-----
}

void process_rcv_packet()
{

}

void process_snd_packet()
{

}


void rcv_packet_tcp(t_data_sckt_buf* data_sckt_buf)
{
	char* tcp_row_packet=NULL;

	u32 ack_num=GET_DWORD(tcp_row_packet[8],tcp_row_packet[9],tcp_row_packet[10],tcp_row_packet[11]);
	is_ack=tcp_row_packet[8] & 0x10;


	tcp_queue_add(t_tcp_queue* tcp_queue,t_data_sckt_buf* data_sckt_buf,u8 status)

	----



	tcp_row_packet=data_sckt_buf->transport_hdr;

	if (ack) 
	{
		add ack queue
	}
	else if (packet data)
	{
		add rcv queue
	}
}

void snd_packet_tcp(char* data,int data_len........)
{
	char* tcp_payload;
	t_data_sckt_buf* data_sckt_buf;
	
	data_sckt_buf=alloc_sckt(data_len+HEADER_ETH+HEADER_IP4+HEADER_TCP);
	data_sckt_buf->transport_hdr=data_sckt_buf->data+HEADER_ETH+HEADER_IP4;
	tcp_payload=data_sckt_buf->transport_hdr+HEADER_TCP;
	kmemcpy(tcp_payload,data,data_len);
	tcp_header=data_sckt_buf->transport_hdr;
	
	tcp_header[0]=HI_16(src_port);   		//HI SRC PORT
	tcp_header[1]=LOW_16(src_port);  		//LOW SRC PORT
	tcp_header[2]=HI_16(src_port);   		//HI DST PORT
	tcp_header[3]=LOW_16(src_port);  		//LOW DST PORT

	tcp_header[4]=HI_OCT_32(seq_num);         	//HIGH SEQ NUM
	tcp_header[5]=MID_LFT_OCT_32(seq_num);    	//MID LEFT SEQ NUM
	tcp_header[6]=MID_RGT_OCT_32(seq_num);    	//MID RIGHT SEQ NUM
	tcp_header[7]=LOW_OCT_32(seq_num);        	//LOW SEQ NUM

	tcp_header[8]=HI_OCT_32(ack_num);		//HIGH ACK NUM
	tcp_header[9]=MID_LFT_OCT_32(ack_num);		//MID LEFT ACK NUM
	tcp_header[10]=MID_RGT_OCT_32(ack_num); 	//MID RIGHT ACK NUM
	tcp_header[11]=LOW_OCT_32(ack_num);		//LOW ACK NUM

	tcp_header[12]=head_len << 4;                   //HEADER LEN + 4 RESERVED BIT
	tcp_header[13]=flags;                           //FLAGS
	tcp_header[14]=HI_16(win_size);			//HI WINDOW SIZE
	tcp_header[15]=LOW_16(win_size);                //LOW WINDOW SIZE

	tcp_header[16]=HI_16(checksum);			//HI TCP CHECKSUM
	tcp_header[17]=LOW_16(checksum);		//LOW TCP CHECKSUM
	tcp_header[18]=0;				//HI URGENT POINTER (NOT USED)
	tcp_header[19]=0;				//LOW URGENT POINTER (NOT USED)
}


