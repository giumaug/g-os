//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANTE VERIFICARE CHE LA STRUTTURA DATI COMPLESSIVA SIA SENSATA!!!!!!!!!!!!!!!!!!

#define TCP_RCV_SIZE 
#define TCP_SND_SIZE
#define TCP_CONN_MAP_SIZE 	20

#define INC_WND(cur,wnd_size,offset)  (cur + offset) % wnd_size
#define SLOT_WND (cur) (cur) % wnd_size

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
	u32 conn_id;
	t_tcp_queue* rcv_buf;
	t_tcp_queue* snd_buf;
	u32 ack_seq_num;
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

t_tcp_conn_desc* tcp_conn_desc_int(u16 src_port,u16 dst_port)
{
	t_tcp_conn_desc* tcp_conn_desc;

	tcp_conn_desc=kmalloc(sizeof(t_tcp_conn_desc));
	tcp_conn_desc->rcv_buf=tcp_queue_init(TCP_RCV_SIZE);
	tcp_conn_desc->snd_buf=tcp_queue_init(TCP_SND_SIZE);
	tcp_conn_desc->conn_id=src_port | (dst_port<<16);
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

int tcp_queue_add(t_tcp_queue* tcp_queue,u32 seq_num,t_data_sckt_buf* data_sckt_buf,u8 status)
{
	u32 cur;

	cur=SLOT_WND(seq_num);
	if (cur<tcp_queue->min || cur>tcp_queue->max)
	{
		return -1;
	}
	t_packet packet=kmalloc(sizeof(t_packet));
	packet->status=status;
	packet->val=data_sckt_buf;
	tcp_queue->buf[cur]=data_sckt_buf;
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

static int paket_in_window(min,max,index)
{
	if (min <= max)
	{
		if (index >= min && index <= max)
		{
			return 1;
		}
	}
	else if ( min > max)
	{
		if (index >= max && index <= min)
		{
			return 1;
		}
	}
	return 0;
}

static void update_window_and_ack(t_tcp_queue* tcp_queue)
{
	u32 ack_seq_num;

	index=tcp_queue->max;
	while(index!=tcp_queue->max)
	{	
		packet=tcp_queue->buf[index];
		if (packet==NULL || packet->status!=1)
		{
			break;
		}
		seq_num=GET_DWORD(tcp_row_packet[4],tcp_row_packet[5],tcp_row_packet[6],tcp_row_packet[7]);
		index=(index+1) % TCP_RCV_SIZE;
	}
	if (index > tcp_queue->max) 
	{
		tcp_conn_desc->ack_seq_num=seq_num+1;//next to ack
		tcp_conn_desc->min=ack_seq_num=seq_num+1;
		tcp_conn_desc->max=------------qui okkio approssimazione ==
	}

}


void rcv_packet_tcp(t_data_sckt_buf* data_sckt_buf)
{
	t_tcp_desc* tcp_desc=NULL;;
	t_tcp_conn_desc* tcp_conn_desc=NULL;
	char* tcp_row_packet=NULL;
	u16 src_port;
	u16 sdst_port;
	u32 conn_id;
	u32 seq_num;

	tcp_desc=system.network_desc->tcp_desc;
	tcp_row_packet=data_sckt_buf->transport_hdr;
	src_port=GET_WORD(tcp_row_packet[0],tcp_row_packet[1]);
	dst_port=GET_WORD(tcp_row_packet[2],tcp_row_packet[3]);
	conn_id=dst_port | (src_port<<16);
	tcp_conn_desc=hashtable_get(tcp_desc->conn_map,conn_id);
	if (tcp_conn_desc==NULL) 
	{
		return NULL;
	}
	seq_num=GET_DWORD(tcp_row_packet[4],tcp_row_packet[5],tcp_row_packet[6],tcp_row_packet[7]);
	index=SLOT_WND(seq_num);
	t_tcp_queue* tcp_queue=tcp_conn_desc->rcv_buf;
	
	if paket_in_window(tcp_queue->min,tcp_queue->max,index)
	{
		update_window_and_ack();
	}




	
	tcp_queue_add(tcp_conn_desc->rcv_buf,seq_num,data_sckt_buf,0);???
	
	t_tcp_queue* tcp_queue=tcp_conn_desc->rcv_buf;
	u8 check_queue=1;
	prv_seq_num=seq_num-1;
	nxt_seq_num=0;
	while(1)
	{
		index=SLOT_WND(prv_seq_num);
		if (index<tcp_queue->min)
		{
			break;
		}
		packet=tcp_queue->buf[index];
		if (packet==NULL || packet->status!=1)
		{
			break;
		}
		prv_seq_num--;
	}
	if (SLOT_WND(prv_seq_num+1)==tcp_queue->min)
	{
		nxt_seq_num=seq_num+1;
		while (1)
		{
			index=SLOT_WND(nxt_seq_num);
			if (index<tcp_queue->min)
			{
				break;
			}
			packet=tcp_queue->buf[index];
			if (packet==NULL || packet->status!=1)
			{
				break;
			}
			nxt_seq_num++;
		}
	}
	if (nxt_seq_num!=0)
	{
		tcp_conn_desc->ack_seq_num=nxt_seq_num;//next to ack
		tcp_conn_desc->min=
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


