//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANTE VERIFICARE CHE LA STRUTTURA DATI COMPLESSIVA SIA SENSATA!!!!!!!!!!!!!!!!!!

#define TCP_RCV_SIZE 
#define TCP_SND_SIZE
#define TCP_CONN_MAP_SIZE 	20

#define INC_WND(cur,wnd_size,offset)  (cur + offset) % wnd_size
#define SLOT_WND (cur,wnd_size) (cur % wnd_size

typedef struct s_packet
{
	void* val;
	u8 status;
}
t_packet;


typedef struct s_sldw_buf
{
	u32 min;
	u32 max;
	u32 cur; //first slow inside trassmission window free (no packet waiting ack)
	u32 size;
	char* buf;
}
t_sldw_buf;

typedef struct s_tcp_snd_queue
{
	t_sldw_buf data;
	u32 nxt_snd;
}
t_tcp_snd_queue;

typedef struct s_tcp_rcv_queue
{
	t_sldw_buf in_order_data;
	t_sldw_buf out_order_data;
	u32 nxt_rcv;
}
t_tcp_snd_queue;

typedef struct s_tcp_conn_desc
{
	u32 conn_id;
	t_tcp_rcv_queue* rcv_queue;
	t_tcp_snd_queue* snd_queue;
	u32 ack_seq_num;
	u32 offered_ack; //ack relativo finestra di ricezione
	u32 expected_ack; // ack relativo finestra trasmissione
}
t_tcp_conn_desc;

typedef struct s_tcp_desc
{
	t_hashtable* conn_map;
}
t_tcp_desc;

t_sldw_buf* sldw_buf_init(u32 size,u32 mask_size)
{
	int i;
	t_sldw_buf* sldw_buf;

	sldw_buf=kmalloc(sizeof(t_sldw_buf));
	sldw_buf->buf=kmalloc(size);
	sldw_buf->rcv_min=0;
	sldw_buf->rcv_max=0;
	sldw_buf->cur=0;
	sldw_buf->size=size;
	return sldw_buf;
}

void sldw_buf_free(tcp_queue* tcp_queue)
{
	kfree(sldw_buf->buf);
	kfree(sldw_buf);
}

t_tcp_snd_queue* tcp_snd_queue_init(u32 size)
{
	t_tcp_snd_queue* tcp_snd_queue;

	tcp_snd_queue=kmalloc(sizeof(t_tcp_snd_queue));
	tcp_snd_queue->data=sldw_buf_init(size);
	tcp_snd_queue->nxt_snd=0;
	return tcp_snd_queue;
}

void tcp_snd_queue_free(t_tcp_snd_queue* tcp_snd_queue)
{
	sldw_buf_init(tcp_snd_queue->data);
	kfree(tcp_snd_queue);
}

t_tcp_rcv_queue* tcp_rcv_queue_init(u32 size)
{
	t_tcp_rcv_queue* tcp_rcv_queue;

	tcp_rcv_queue=kmalloc(sizeof(t_tcp_rcv_queue));
	tcp_rcv_queue->in_order_data=sldw_buf_init(size);
	tcp_rcv_queue->out_order_data=sldw_buf_init(size);
	tcp_rcv_queue->nxt_rcv=0;
	return tcp_rcv_queue;
}

void tcp_rcv_queue_free(t_tcp_rcv_queue* tcp_rcv_queue)
{
	sldw_buf_init(tcp_rcv_queue->in_order_data);
	sldw_buf_init(tcp_rcv_queue->out_order_data);
	kfree(tcp_rcv_queue);
}

t_tcp_conn_desc* tcp_conn_desc_int(u16 src_port,u16 dst_port)
{
	t_tcp_conn_desc* tcp_conn_desc;

	tcp_conn_desc=kmalloc(sizeof(t_tcp_conn_desc));
	tcp_conn_desc->rcv_buf=tcp_rcv_queue_init(TCP_RCV_SIZE);
	tcp_conn_desc->snd_buf=tcp_snd_queue_init(TCP_SND_SIZE);
	tcp_conn_desc->conn_id=src_port | (dst_port<<16);
	return tcp_conn_desc;
}

void tcp_conn_desc_free(t_tcp_conn_desc* tcp_conn_desc)
{
	tcp_rcv_queue_free(tcp_conn_desc->rcv_buf);
	tcp_snd_queue_free(tcp_conn_desc->snd_buf);
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

static int data_in_window(min,max,index)
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

static void update_rcv_window_and_ack(t_tcp_queue* tcp_queue)
{
	u32 ack_seq_num;
	u32 min;
	u32 offset;

	offset=0;
	index=tcp_queue->rcv_min;
	while(index!=((tcp_queue->rcv_max+1) % TCP_RCV_SIZE))
	{	
		packet=tcp_queue->buf[index];
		if (tcp_queue->buf[index]==0)
		{
			break;
		}
		tcp_queue->rcv_rdy++;
		offset++;
		index=(tcp_queue->rcv_min+offset) % TCP_RCV_SIZE;
	}
	if (index != tcp_queue->min) 
	{
		tcp_queue->offered_ack=index;
		tcp_queue->min=(tcp_queue->min+offset) % TCP_RCV_SIZE;
		tcp_queue->max=(tcp_queue->max+offset) % TCP_RCV_SIZE;
		tcp_queue->offered_wnd=(tcp_queue->max-tcp_queue->min >=0 ? (tcp_queue->max-tcp_queue->min) : (tcp_queue->min-tcp_queue->max));
	}
}

crc???
void rcv_packet_tcp(t_data_sckt_buf* data_sckt_buf)
{
	t_tcp_desc* tcp_desc=NULL;;
	t_tcp_conn_desc* tcp_conn_desc=NULL;
	char* tcp_row_packet=NULL;
	char* ip_row_packet=NULL;
	u16 src_port;
	u16 dst_port;
	u32 conn_id;
	u32 seq_num;
	u32 ack_seq_num;
	u32 data_len;
	u32 len_1;
	u32 len_2;

	tcp_desc=system.network_desc->tcp_desc;
	tcp_row_packet=data_sckt_buf->transport_hdr;
	ip_row_packet=data_sckt_buf->network_hdr;
	src_port=GET_WORD(tcp_row_packet[0],tcp_row_packet[1]);
	dst_port=GET_WORD(tcp_row_packet[2],tcp_row_packet[3]);
	ack_seq_num=GET_DWORD(tcp_row_packet[8],tcp_row_packet[9],tcp_row_packet[10]tcp_row_packet[11]);
	conn_id=dst_port | (src_port<<16);
	tcp_conn_desc=hashtable_get(tcp_desc->conn_map,conn_id);
	if (tcp_conn_desc==NULL) 
	{
		return NULL;
	}
	seq_num=GET_DWORD(tcp_row_packet[4],tcp_row_packet[5],tcp_row_packet[6],tcp_row_packet[7]);
	t_tcp_queue* tcp_queue=tcp_conn_desc->rcv_buf;
	index=SLOT_WND(seq_num);

	ip_len=GET_WORD(ip_row_packet[2],ip_row_packet[3]);
	data_len=ip_len-HEADER_TCP;

	if (data_in_window(tcp_queue->rcv_min,tcp_queue->rcv_max,index) && data_len>0)-------------------------------------------------------------------------qui!!!!!!!!!!!1
	{
		low_index=seq_num;
		hi_index=seq_num+data_len;
		if (SLOT_WND(low_index)<SLOT_WND(hi_index)) 
		{
			kmemcpy(tcp_queue->buf,data_sckt_buf->data,data_len);
		}
		else 
		{
			len_1=tcp_queue->size-index;
			len_2=data_len-len_1;
			kmemcpy(tcp_queue->buf+index,data_sckt_buf->data,len_1);
			kmemcpy(tcp_queue->buf,data_sckt_buf->data+len_1,len_2);
		}
		update_rcv_window_and_ack();
	}
	rcv_ack(ack_seq_num);

}

//	if good ack
//	{
//        	if cwnd<ssthresh cwnd+=SMSS                    //slow start SMSS sender maximum segment size (13.8) usiamo valore
//							       // costante 1454=MTU_ETH-HEADER_TCP-HEADER_IP-HEADER_ETH
//		
//		if cwnd>ssthresh cwnd+=SMSS*(SMSS/cwnd)        //congestion avoidance
//	}
//
//	else if third duplicate ack                            //start fast retrasmit
//	{
//
//		while (!good ack received)
//		{
//			ssthresh=max(flight_size/2,2*SMSS)            //flight size=min(cwnd,awnd)
//
//			retrasmit packet signaled by duplicated ack
//
//			cwnd+=SMSS for each ack duplicated ack received
//		}
//		cwnd=ssthresh
//	}
//	update window edges

void rcv_ack(t_tcp_desc* tcp_desc,u32 ack_seq_num)
{
	if (tcp_conn_desc->min<=ack_seq_num)
	{
		if (tcp_conn_desc->duplicated_ack>0)
		{
			tcp_conn_desc->duplicated_ack=0;
			tcp_conn_desc->cwnd=tcp_conn_desc->ssthresh;
		}
		if (tcp_conn_desc->cwnd<=tcp_conn_desc->ssthresh)
		{
			tcp_conn_desc->cwnd+=SMSS;
		}
		else 
		{
			tcp_conn_desc->cwnd+=SMSS*(SMSS/tcp_conn_desc->cwnd->cwnd);
		}
	}
	else if (++tcp_conn_desc->duplicated_ack==3)
	{
		tcp_conn_desc->ssthresh=max(tcp_conn_desc->flight_size/2,2*SMSS);
		tcp_conn_desc->cwnd+=SMSS;
	}
	tcp_conn_desc->snd_buf->rcv_max=tcp_conn_desc->snd_buf->rcv_min+tcp_conn_desc->cwnd;
	update_snd_window(tcp_conn_desc->snd_buf);
}

static void update_snd_window(t_tcp_queue* tcp_queue,u32 good_ack)
{
	u32 word_to_ack;
	u32 indx;

	if (good_ack!=0)
	{
		word_to_ack = good_ack - tcp_queue->min;
		if (tcp_queue->cur = good_ack)
		{
			tcp_queue->cur = 0;
		}
		tcp_queue->min = tcp_queue->min + word_to_ack;
		tcp_queue->max = tcp_queue->min + tcp_conn_desc->cwnd;
	}

	wnd_r_limit=SLOT_WND(tcp_queue->max+1,tcp_queue->mask_size);
	indx=SLOT_WND(tcp_queue->min,tcp_queue->mask_size);
	min_indx=indx=indx;

	while (tcp_queue->wnd_mask[indx] == 0xff && indx != wnd_r_limit)
	{
		INC_WND(indx,tcp_queue->mask_size,1);
	}
	if (tcp_queue->wnd_mask[indx] != 0)
	{
		if (tcp_queue->wnd_mask[indx]==1)
		{
			offset=1;
		}
		else if (tcp_queue->wnd_mask[indx]==11)
		{
			offset=2;
		}
		else if (tcp_queue->wnd_mask[indx]==111)
		{
			offset=3;
		}
		else if (tcp_queue->wnd_mask[indx]==1111)
		{
			offset=4;
		}
		else if (tcp_queue->wnd_mask[indx]==11111)
		{
			offset=5;
		}
		else if (tcp_queue->wnd_mask[indx]==111111)
		{
			offset=6;
		}
		else if (tcp_queue->wnd_mask[indx]==1111111)
		{
			offset=7;
		}
		INC_WND(indx,tcp_queue->size,offset);
	}

	if (indx == wnd_r_limit)
	{
		return;
	}

	min=

	if (tcp_queue->cur<indx)
	{
		return;
	}
	else if (index=<tcp_queue->cur<=)
	
	




	
	if (i>tcp_queue->min)
	{
		preparo socket buffer compattando su SMSS
		marco nuovo stato slot
		aggiorno timer
	}	
}

void ack_time_out()
{
	cwnd=SMSS
	update window edges	
}

void snd_packet()
{
	- posso inviare un nuovo pachetto?
	- aggiorno trasmission window3
}

void flush_trasmission_window()
{
	-e' possibile trasmettere un pacchetto
		-trasmetti pacchetto
		-aggiorna tramsission windows4
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


