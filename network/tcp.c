//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANTE VERIFICARE CHE LA STRUTTURA DATI COMPLESSIVA SIA SENSATA!!!!!!!!!!!!!!!!!!

#define TCP_RCV_SIZE 
#define TCP_SND_SIZE
#define TCP_CONN_MAP_SIZE 	20

#define INC_WND(cur,wnd_size,offset)  (cur + offset) % wnd_size
#define SLOT_WND (cur,wnd_size) (cur % wnd_size
#define DATA_IN_WND(min,max,index)				\
(								\
	(min <= max) ?						\
	(							\
		(index >= min && index <= max) ? 1 : 0		\
	)							\
	:							\
	(							\
		(index >= max && index <= min) ? 1 : 0		\
	)							\
)								\

#define DATA_LF_OUT_WND(min,max,index) ((index < min) ? 1 : 0)
#define DATA_RH_OUT_WND(min,max,index) ((index > min) ? 1 : 0)

typedef struct s_tcp_snd_queue
{
	u32 min;
	u32 max;
	u32 cur;
	u32 size;
	char* buf;
	u32 nxt_snd;
}
t_tcp_snd_queue;

typedef struct s_tcp_rcv_queue
{
	u32 min;
	u32 max;
	u32 cur;
	u32 size;
	char* in_order_buf;
	char* out_order_buf;
	u32 nxt_rcv; //ack relativo finestra di ricezione e usato in trasmissione
}
t_tcp_snd_queue;

typedef struct s_tcp_conn_desc
{
	u32 conn_id;
	u32 rto;
	u32 timer;
	t_tcp_rcv_queue* rcv_queue;
	t_tcp_snd_queue* snd_queue;
	u32 ack_seq_num;
//	u32 offered_ack; //va usato nxt_rcv
//	u32 expected_ack; // va usato nxt_snd
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
}
t_tcp_conn_desc;

typedef struct s_tcp_desc
{
	t_hashtable* conn_map;
	t_llist* conn_list;
	new_dllist tcp_conn_desc_free(t_tcp_conn_desc* tcp_conn_desc)
}
t_tcp_desc;

t_tcp_snd_queue* tcp_snd_queue_init(u32 size)
{
	t_tcp_snd_queue* tcp_snd_queue;

	tcp_snd_queue=kmalloc(sizeof(t_tcp_snd_queue));
	tcp_snd_queue->buf=kmalloc(size);
	tcp_snd_queue->min=0;
	tcp_snd_queue->max=0;
	tcp_snd_queue->cur=0;
	tcp_snd_queue->size=size;
	tcp_snd_queue->nxt_snd=0;
	return tcp_snd_queue;
}

void tcp_snd_queue_free(t_tcp_snd_queue* tcp_snd_queue)
{
	kfree(tcp_snd_queue->buf);
	kfree(tcp_snd_queue);
}

t_tcp_rcv_queue* tcp_rcv_queue_init(u32 size)
{
	t_tcp_rcv_queue* tcp_rcv_queue;

	tcp_rcv_queue=kmalloc(sizeof(t_tcp_rcv_queue));
	tcp_rcv_queue->in_order_buf=kmalloc(size);
	tcp_rcv_queue->out_order_buf=kmalloc(size/8);
	tcp_rcv_queue->min=0;
	tcp_rcv_queue->max=0;
	tcp_rcv_queue->cur=0;
	tcp_rcv_queue->size=size;
	tcp_rcv_queue->nxt_rcv=0;
	return tcp_rcv_queue;
}

void tcp_rcv_queue_free(t_tcp_rcv_queue* tcp_rcv_queue)
{
	kfree(tcp_rcv_queue->in_order_buf);
	kfree(tcp_rcv_queue->out_order_buf);
	kfree(tcp_rcv_queue);
}

t_tcp_conn_desc* tcp_conn_desc_int(u16 src_port,u16 dst_port)
{
	t_tcp_conn_desc* tcp_conn_desc;

	tcp_conn_desc=kmalloc(sizeof(t_tcp_conn_desc));
	tcp_conn_desc->rcv_buf=tcp_rcv_queue_init(TCP_RCV_SIZE);
	tcp_conn_desc->snd_buf=tcp_snd_queue_init(TCP_SND_SIZE);
	tcp_conn_desc->conn_id=src_port | (dst_port<<16);
	tcp_conn_desc_timer=0xFFFFFFFF;
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
	tcp_desc->conn_map=dc_hashtable_init(TCP_CONN_MAP_SIZE,&tcp_conn_desc_free);
	tcp_desc->conn_map=dc_new_dllist(tcp_conn_desc_free);
	return tcp_desc;
}

void tcp_free(t_tcp_desc* tcp_desc)
{
	hashtable_free(tcp_desc->conn_map);
	free_llist(tcp_desc->conn_list);
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
		tcp_queue->offered_ack=index;//no!!!
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
	t_tcp_rcv_queue* tcp_queue=tcp_conn_desc->rcv_buf;
	index=SLOT_WND(seq_num);

	ip_len=GET_WORD(ip_row_packet[2],ip_row_packet[3]);
	data_len=ip_len-HEADER_TCP;

	if (DATA_IN_WND(tcp_queue->rcv_min,tcp_queue->rcv_max,index) && data_len>0)
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
	rcv_ack(tcp_conn_desc,ack_seq_num);

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

void rcv_ack(t_tcp_conn_desc* tcp_conn_desc,u32 ack_seq_num)
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
	tcp_conn_desc->snd_buf->max=tcp_conn_desc->snd_buf->min+tcp_conn_desc->cwnd;
	update_snd_window(tcp_conn_desc->snd_buf,ack_seq_num);
}

offered_ack su send????------------------------qui
static void update_snd_window(t_tcp_conn_desc* tcp_conn_desc,u32 ack_seq_num)
{
	u32 word_to_ack;
	u32 indx;
	t_tcp_snd_queue* tcp_queue = NULL;

	tcp_queue = tcp_conn_desc->snd_queue;
	//trasmission with good ack
	if (tcp_conn_desc->duplicated_ack == 0)
	{
		word_to_ack = good_ack - tcp_queue->min;
		tcp_queue->data->min = tcp_queue->data->min + word_to_ack;
		tcp_queue->data->max = tcp_queue->data->min + tcp_conn_desc->cwnd;
		data_to_send=tcp_queue->tcp_queue->data->max-tcp_queue->nxt_snd-1;

		wnd_l_limit = SLOT_WND(tcp_snd_queue->nxt_snd-1,tcp_queue->size);
		wnd_r_limit = SLOT_WND(tcp_queue->max,tcp_queue->size);

	 	if (DATA_LF_OUT_WND(wnd_l_limit,wnd_r_limit,tcp_queue->cur))
		{
		 	//no data to send
			return;
		}
		if (DATA_IN_WND(wnd_l_limit,wnd_r_limit,tcp_queue->cur))
		{	
			//in window
			wnd_r_limit=tcp_queue->cur:
		}
		//more data than window nothing to do

		//sender silly window avoidance		
		w_size=WND_SIZE(wnd_l_limit,wnd_r_limit);
		if (tcp_conn_desc->rcv_queue->nxt_rcv == 0 || w_size >= SMSS || w_size >= tcp_conn_desc->max_adv_wnd/2)
		{
			data_to_send=w_size;
		}
		else if (w_size >= SMSS)
		{
			data_to_send = w_size - (w_size % SMSS);
		}
		else if (w_size >= tcp_conn_desc->max_adv_wnd/2)
		{
			data_to_send=w_size;
		}
		indx=wnd_l_limit;
		tcp_queue->nxt_snd += data_to_send;
	}
	else if (tcp_conn_desc->duplicated_ack == 1 || tcp_conn_desc->duplicated_ack == 2)
	{
		wnd_l_limit = SLOT_WND(tcp_snd_queue->nxt_snd-1,tcp_queue->size);
		wnd_r_limit = SLOT_WND(tcp_queue->max,tcp_queue->size);
		w_size=WND_SIZE(wnd_l_limit,wnd_r_limit);
		flight_size = WND_SIZE(tcp_snd_queue->min,tcp_snd_queue->nxt_snd-1);
		flight_size_limit = tcp_conn_desc->cwnd + 2*SMSS;
		
		if (w_size >= SMSS && (flight_size + SMSS <= flight_size_limit))
		{
			indx = SLOT_WND(tcp_snd_queue->nxt_snd,tcp_queue->size);
			tcp_snd_queue->nxt_snd += SMSS;
			data_to_send = SMSS;
		}
	}
	else if (tcp_conn_desc->duplicated_ack == 3)
	{
		indx = SLOT_WND(ack_seq_num,tcp_queue->size);
		tcp_snd_queue->nxt_snd += SMSS;
		data_to_send = SMSS;
	}
	else if (tcp_conn_desc->duplicated_ack > 3)
	{
		indx = SLOT_WND(tcp_snd_queue->nxt_snd,tcp_queue->size);
		wnd_l_limit = SLOT_WND(tcp_snd_queue->nxt_snd-1,tcp_queue->size);
		wnd_r_limit = SLOT_WND(tcp_queue->max,tcp_queue->size);
		w_size=WND_SIZE(wnd_l_limit,wnd_r_limit);

		if (w_size >= SMSS)
		{
			tcp_snd_queue->nxt_snd += SMSS;
			data_to_send = SMSS;
		}
	}

	if (data_to_send > 0)
	{
		while (data_to_send >= SMSS)
		{
			send_packet_tcp(tcp_queue->buf[indx],
					SMSS,
					tcp_conn_desc->src_ip,
					tcp_conn_desc->dst_ip,
					tcp_conn_desc->src_port,
					tcp_conn_desc->dst_port);

			data_to_send -= SMSS;
			indx += SMSS;
		}
		if (data_to_send > 0 )
		{	
			send_packet_tcp(tcp_queue->buf[indx],
					data_to_send,
					tcp_conn_desc->src_ip,
					tcp_conn_desc->dst_ip,
					tcp_conn_desc->src_port,
					tcp_conn_desc->dst_port);
		}
		//timer RFC6298
		if (tcp_conn_desc->timer == 0xFFFFFFFF)
		{
			tcp_conn_desc->timer = tcp_conn_desc->rto;
		}
	}
}

//void send_packet_tcp(indx,data_to_send,?????);
void send_packet_tcp(u32 indx,u32 data_to_send)
{
	x
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


int snd_packet_tcp(char* data,u32 data_len,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port)
{
	char* tcp_payload = NULL;
	t_data_sckt_buf* data_sckt_buf = NULL;
	int ret = NULL;
	
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

	ret=send_packet_ip4(data_sckt_buf,src_ip,dst_ip,ip_packet_len,TCP_PROTOCOL);
	return ret;
}


