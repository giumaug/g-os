//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANTE VERIFICARE CHE LA STRUTTURA DATI COMPLESSIVA SIA SENSATA!!!!!!!!!!!!!!!!!!
#include "network/tcp.h"

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
	tcp_conn_desc->rtrsn_timer=0xFFFFFFFF;
	tcp_conn_desc->pgybg_timer=0xFFFFFFFF;
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

static void update_rcv_window_and_ack(t_tcp_rcv_queue* tcp_queue,rcv_seq_num,u32 data_len)
{
	if (tcp_queue->wnd_min == rcv_seq_num)
	{
		tcp_queue->wnd_min += data_len;
		tcp_queue->wnd_max += data_len;

	}
}



static void _update_rcv_window_and_ack(t_tcp_rcv_queue* tcp_queue,u32 rcv_seq_num)
{
	u32 ack_seq_num;
	u32 min;
	u32 offset;

	offset=0;
	index=tcp_queue->wnd_min;
	while(index <= tcp_queue->wnd_max)
	{
		slot_state = tcp_queue->out_order_buf[index/8];
		if (slot_state == 0)
		{
			break;
		}
		index++;
	}
	if (index != tcp_queue->min) 
	{
		if (slot_state == 0xFF)
		{
			tcp_queue->min += index;
			tcp_queue->man += index;
			tcp_queue->wnd_size = tcp_queue->wnd_max - tcp_queue->wnd_min;
			for (i = 0;i <= 
		}
		tcp_queue->offered_ack=index;//no!!!
		tcp_queue->min=(tcp_queue->min+offset) % TCP_RCV_SIZE;
		tcp_queue->max=(tcp_queue->max+offset) % TCP_RCV_SIZE;
		tcp_queue->offered_wnd=(tcp_queue->max-tcp_queue->min >=0 ? (tcp_queue->max-tcp_queue->min) : (tcp_queue->min-tcp_queue->max));
	}
}

rcv_packet_tcp(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len)
{
	t_tcp_desc* tcp_desc=NULL;
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

	if (checksum_udp((unsigned short*) tcp_row_packet,src_ip,dst_ip,data_len)==0)-----------qui
	{
		if (seq_num >= tcp_queue->wnd_min && seq_num +data_len <= tcp_queue->wnd_max)
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
	else
	{
		free_sckt(data_sckt_buf);
	}

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

static void update_snd_window(t_tcp_conn_desc* tcp_conn_desc,u32 ack_seq_num)
{
	u32 word_to_ack;
	u32 expected_ack;
	u32 indx;
	t_tcp_snd_queue* tcp_queue = NULL;
	u8 flags=0;

	tcp_queue = tcp_conn_desc->snd_queue;
	//trasmission with good ack
	if (tcp_conn_desc->duplicated_ack == 0)
	{
		word_to_ack = ack_seq_num - tcp_queue->min;
		tcp_queue->wnd_min = tcp_queue->wnd_min + word_to_ack;
		tcp_queue->wnd_max = tcp_queue->wnd_min + tcp_conn_desc->cwnd;
		data_to_send=tcp_queue->tcp_queue->data->wnd_max-tcp_queue->nxt_snd-1;
		tcp_queue->buf_min = tcp_queue->wnd_min;
		INC_WND(tcp_queue->buf_max,tcp_queue->buf_size,word_to_ack);
		
		wnd_l_limit = SLOT_WND(tcp_snd_queue->nxt_snd,tcp_queue->size);
		wnd_r_limit = SLOT_WND(tcp_queue->wnd_max,tcp_queue->size);

	 	if (DATA_LF_OUT_WND(wnd_l_limit,wnd_r_limit,tcp_queue->buf_cur))
		{
		 	//no data to send
			if (tcp_conn_desc->pgybg_timer == 0xFFFFFFFF)
			{
				tcp_conn_desc->pgybg_timer = PIGGYBACKING_TIMEOUT;
			}
			return;
		}
		if (DATA_IN_WND(wnd_l_limit,wnd_r_limit,tcp_queue->buf_cur))
		{	
			//in window
			wnd_r_limit=tcp_queue->buf_cur:
		}
		//more data than window nothing to do

		//sender silly window avoidance		
		w_size=WND_SIZE(wnd_l_limit,wnd_r_limit);
		expected_ack = tcp_queue->nxt_snd - tcp_queue->data->wnd_min;
		if (expected_ack == 0 || w_size >= SMSS || w_size >= tcp_conn_desc->max_adv_wnd/2)
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
		tcp_desc->seq_num = tcp_queue->nxt_snd;
		tcp_queue->nxt_snd += data_to_send;
	}
	else if (tcp_conn_desc->duplicated_ack == 1 || tcp_conn_desc->duplicated_ack == 2)
	{
		wnd_l_limit = SLOT_WND(tcp_snd_queue->nxt_snd-1,tcp_queue->size);
		wnd_r_limit = SLOT_WND(tcp_queue->wnd_max,tcp_queue->size);
		w_size=WND_SIZE(wnd_l_limit,wnd_r_limit);
		flight_size = WND_SIZE(tcp_snd_queue->wnd_min,tcp_snd_queue->nxt_snd-1);
		flight_size_limit = tcp_conn_desc->cwnd + 2*SMSS;
		
		if (w_size >= SMSS && (flight_size + SMSS <= flight_size_limit))
		{
			indx = SLOT_WND(tcp_snd_queue->nxt_snd,tcp_queue->buf_size);
			tcp_desc->seq_num = tcp_queue->nxt_snd;
			tcp_snd_queue->nxt_snd += SMSS;
			data_to_send = SMSS;
		}
	}
	else if (tcp_conn_desc->duplicated_ack == 3)
	{
		indx = SLOT_WND(ack_seq_num,tcp_queue->size);
		tcp_desc->seq_num = ack_seq_num;
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
			tcp_desc->seq_num = tcp_queue->nxt_snd;
			tcp_snd_queue->nxt_snd += SMSS;
			data_to_send = SMSS;
		}
	}

	if (data_to_send > 0)
	{
		ack_num = tcp_conn_desc->rcv_queue->nxt_rcv;
		if (if (ack_num > 0)
		{
			data_len =  data_to_send >= SMSS ? SMSS : data_to_send;
			flags = FLG_ACK;
			tcp_conn_desc->rcv_queue->nxt_rcv = 0;
			send_packet_tcp(tcp_conn_desc,tcp_queue->buf[indx],data_len,ack_num,flags);
			data_to_send -= data_len;
			indx += data_len;
			
			if (tcp_conn_desc->pgybg_timer != 0xFFFFFFFF)
			{
				tcp_conn_desc->pgybg_timer = 0xFFFFFFFF;
			}
		}

		while (data_to_send >= SMSS)
		{
			send_packet_tcp(tcp_conn_desc,tcp_queue->buf[indx],SMSS,ack_num,flags);
			data_to_send -= SMSS;
			indx += SMSS;
		}
		if (data_to_send > 0)
		{	
			send_packet_tcp(tcp_conn_desc,tcp_queue->buf[indx],data_to_send,0,flags);
		}
		//timer RFC6298
		if (tcp_conn_desc->rtrsn_timer == 0xFFFFFFFF)
		{
			tcp_conn_desc->rtrsn_timer = tcp_conn_desc->rto;
		}
	}
}

//Meglio scodare solo dentro la deferred queue,serve pure un meccanismo che
//avvia lo scodamentom solo se non e' stato lanciato in precedenza dal
//processamento dell'ack
int buffer_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len)
{
	t_tcp_snd_queue* tcp_queue = tcp_conn_desc->snd_queue;

	b_free_size=WND_SIZE(tcp_queue->tcp_snd_queue->nxt_snd,tcp_queue->buf_max);
	if (b_free_size < data_len)
	{
		return -1;
	}
	if (tcp_snd_queue->cur < tcp_queue->buf_max) 
	{
		kmemcpy(tcp_queue->buf[tcp_snd_queue->cur],data,data_len);
		INC_WND(tcp_snd_queue->cur,tcp_snd_queue->buf_size,data_len);
	}
	else
	{
		if ((tcp_queue->buf_size - tcp_queue->buf_cur) > data_len)
		{
			kmemcpy(tcp_queue->buf[tcp_snd_queue->cur],data,data_len);
			INC_WND(tcp_snd_queue->cur,tcp_snd_queue->buf_size,data_len);
		}
		else
		{
			offset = data_len - (tcp_queue->buf_size - tcp_queue->buf_cur);
			kmemcpy(tcp_queue->buf[tcp_snd_queue->cur],data,data_len);
			kmemcpy(tcp_queue->buf[0],data,offset);
			INC_WND(tcp_snd_queue->cur,tcp_snd_queue->buf_size,data_len);
		}
	}
	//vedi commento sopra
	//update_snd_window(tcp_conn_desc,0,1);
}

//Non occorre sincronizzazione sto dentro stessa deferred queue
void static pgybg_timer_handler()
{
	u8 flags = 0;
	u32 ack_num = 0;

	flags = FLG_ACK;
	ack_num = tcp_conn_desc->rcv_queue->nxt_rcv;	
	tcp_conn_desc->rcv_queue->nxt_rcv = 0;			
	send_packet_tcp(tcp_conn_desc,NULL,0,ack_num,flags);
	tcp_conn_desc->pgybg_timer = 0xFFFFFFFF;		
}

//tcp_conn_desc->win_size ???
int send_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len,u32 ack_num,u8 flags)
{
	u16 chk;
	char* tcp_payload = NULL;
	t_data_sckt_buf* data_sckt_buf = NULL;
	int ret = NULL;
	char* tcp_header = NULL;
	
	data_sckt_buf=alloc_sckt(data_len+HEADER_ETH+HEADER_IP4+HEADER_TCP);
	data_sckt_buf->transport_hdr=data_sckt_buf->data+HEADER_ETH+HEADER_IP4;
	tcp_payload=data_sckt_buf->transport_hdr+HEADER_TCP;
	kmemcpy(tcp_payload,data,data_len);
	tcp_header=data_sckt_buf->transport_hdr;
	
	tcp_header[0]=HI_16(tcp_conn_desc->src_port);   		//HI SRC PORT
	tcp_header[1]=LOW_16(tcp_conn_desc->src_port);  		//LOW SRC PORT
	tcp_header[2]=HI_16(tcp_conn_desc->dst_port);   		//HI DST PORT
	tcp_header[3]=LOW_16(tcp_conn_desc->dst_port);  		//LOW DST PORT

	tcp_header[4]=HI_OCT_32(tcp_conn_desc->seq_num);         	//HIGH SEQ NUM
	tcp_header[5]=MID_LFT_OCT_32(tcp_conn_desc->seq_num);    	//MID LEFT SEQ NUM
	tcp_header[6]=MID_RGT_OCT_32(tcp_conn_desc->seq_num);    	//MID RIGHT SEQ NUM
	tcp_header[7]=LOW_OCT_32(tcp_conn_desc->seq_num);        	//LOW SEQ NUM

	tcp_header[8]=HI_OCT_32(ack_num);				//HIGH ACK NUM
	tcp_header[9]=MID_LFT_OCT_32(ack_num);				//MID LEFT ACK NUM
	tcp_header[10]=MID_RGT_OCT_32(ack_num); 			//MID RIGHT ACK NUM
	tcp_header[11]=LOW_OCT_32(ack_num);				//LOW ACK NUM

	tcp_header[12]=0x50;	                   			//HEADER LEN + 4 RESERVED BIT (5 << 4)
	tcp_header[13]=flags;                           		//FLAGS
	tcp_header[14]=HI_16(tcp_conn_desc->win_size);			//HI WINDOW SIZE
	tcp_header[15]=LOW_16(tcp_conn_desc->win_size);                	//LOW WINDOW SIZE

	tcp_header[16]=HI_16(checksum);					//HI TCP CHECKSUM
	tcp_header[17]=LOW_16(checksum);				//LOW TCP CHECKSUM
	tcp_header[18]=0;						//HI URGENT POINTER (NOT USED)
	tcp_header[19]=0;						//LOW URGENT POINTER (NOT USED)

	chk=SWAP_WORD(checksum_tcp((unsigned short*) tcp_header,tcp_conn_desc->src_ip,tcp_conn_desc->dst_ip,data_len));
	tcp_header[18]=HI_16(chk);
	tcp_header[19]=LOW_16(chk);

	ret=send_packet_ip4(data_sckt_buf,
			    tcp_conn_desc->src_ip,
			    tcp_conn_desc->dst_ip,
			    tcp_conn_desc->ip_packet_len,
			    TCP_PROTOCOL);
	return ret;
}

static u16 checksum_tcp(char* tcp_row_packet,u32 src_ip,u32 dst_ip,u16 data_len)
{
	u16 packet_len;
	u16 chk;
	u16 chk_virt;
	unsigned char header_virt[16];
	unsigned char chk_final[4];
	
	header_virt[0]=IP_HI_OCT(src_ip);          	
	header_virt[1]=IP_MID_LFT_OCT(src_ip);
	header_virt[2]=IP_MID_RGT_OCT(src_ip);
	header_virt[3]=IP_LOW_OCT(src_ip);
	header_virt[4]=IP_HI_OCT(dst_ip);          	
	header_virt[5]=IP_MID_LFT_OCT(dst_ip);
	header_virt[6]=IP_MID_RGT_OCT(dst_ip);
	header_virt[7]=IP_LOW_OCT(dst_ip);		
	header_virt[8]=0;
	header_virt[9]=TCP_PROTOCOL;
	header_virt[10]=tcp_row_packet[4];
	header_virt[11]=tcp_row_packet[5];

	packet_len=HEADER_TCP+data_len;	
	chk=checksum((unsigned short*) udp_row_packet,packet_len);
	chk_virt=checksum((unsigned short*) header_virt,12);

	chk_final[0]=LOW_16(~chk_virt);
	chk_final[1]=HI_16(~chk_virt);
	chk_final[2]=LOW_16(~chk);
	chk_final[3]=HI_16(~chk);
	return checksum(chk_final,4);
}


