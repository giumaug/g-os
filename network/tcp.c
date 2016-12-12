//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANTE VERIFICARE CHE LA STRUTTURA DATI COMPLESSIVA SIA SENSATA!!!!!!!!!!!!!!!!!!
#include "network/tcp.h"

static int free_port_search()
{
	int i;
	void* port = NULL;
	tcp_desc = system.network_desc->tcp_desc;

	for (i=0;i<32767;i++)
	{
		if (tcp_desc->listen_port_index++ > 65535) 
		{
			tcp_desc->listen_port_index = 32767;
		}
		tmp = tcp_conn_map_get(tcp_desc->listen_map,system.network_desc->ip,0,tcp_desc->listen_port_index,0);
		if (tmp == NULL)
		{
			return tcp_desc->listen_port_index;
		}
	}
	return 0;
}

t_tcp_snd_queue* tcp_snd_queue_init()
{
	t_tcp_snd_queue* tcp_snd_queue = NULL;

	tcp_snd_queue = kmalloc(sizeof(t_tcp_snd_queue));
	tcp_snd_queue->buf = kmalloc(TCP_SND_SIZE);
	tcp_snd_queue->wnd_min = 0;
	tcp_snd_queue->wnd_size = SMSS;
	tcp_snd_queue->buf_min = 0;
	tcp_snd_queue->buf_cur = 0;
	tcp_snd_queue->buf_size = TCP_SND_SIZE;
	tcp_snd_queue->seq_num = 0;
	tcp_snd_queue->nxt_snd = 0;
}

void tcp_snd_queue_free(t_tcp_snd_queue* tcp_snd_queue)
{
	kfree(tcp_snd_queue->buf);
	kfree(tcp_snd_queue);
}

t_tcp_rcv_queue* tcp_rcv_queue_init(u32 size)
{
	t_tcp_rcv_queue* tcp_rcv_queue;

	tcp_rcv_queue = kmalloc(sizeof(t_tcp_rcv_queue));
	tcp_rcv_queue->buf = kmalloc(TCP_RCV_SIZE);
	tcp_rcv_queue->buf_state = bit_vector_init(TCP_RCV_SIZE);
	tcp_rcv_queue->buf_min = 0;
	tcp_rcv_queue->wnd_min = 0;
	tcp_rcv_queue->buf_size = TCP_RCV_SIZE;
	tcp_rcv_queue->nxt_rcv = 0; 
	tcp_rcv_queue->wnd_adv_size = WND_ADV;
	return tcp_rcv_queue;
}

void tcp_rcv_queue_free(t_tcp_rcv_queue* tcp_rcv_queue)
{
	bit_vector_free(tcp_rcv_queue->bit_vector);
	kfree(tcp_rcv_queue->buf);
	kfree(tcp_rcv_queue);
}

t_tcp_conn_desc* tcp_conn_desc_int()
{
	t_tcp_conn_desc* tcp_conn_desc;

	tcp_conn_desc = kmalloc(sizeof(t_tcp_conn_desc));
	tcp_conn_desc->rcv_buf = tcp_rcv_queue_init(TCP_RCV_SIZE);
	tcp_conn_desc->snd_buf = tcp_snd_queue_init(TCP_SND_SIZE);
	tcp_conn_desc->rto = 0;
	tcp_conn_desc->rtrsn_timer = timer_init(0,&rtrsn_timer_handler,tcp_conn_desc,0);
	tcp_conn_desc->pgybg_timer = tmer_init(PIGGYBACKING_TIMEOUT,&pgybg_timer_handler,tcp_conn_desc,0);
	tcp_conn_desc->rto = DEFAULT_RTO;	
	tcp_conn_desc->seq_num = 0;
	tcp_conn_desc->wnd_cwnd = SMSS;
	tcp_conn_desc->ssthresh = WND_ADV;
	tcp_conn_desc->src_ip = 0;
	tcp_conn_desc->dst_ip = 0;
	tcp_conn_desc->src_port = 0;
	tcp_conn_desc->dst_port = 0;
	tcp_conn_desc->back_log_i_queue = new_queue(&tcp_conn_desc_free);
	tcp_conn_desc->back_log_c_queue = new_queue(&tcp_conn_desc_free);
	SPINLOCK_INIT(tcp_conn_desc->lock);
	tcp_conn_desc->rcv_queue = new_queue();
	tcp_conn_desc->status = OPEN;
	return tcp_conn_desc;
}

void tcp_conn_desc_free(t_tcp_conn_desc* tcp_conn_desc)
{
	tcp_rcv_queue_free(tcp_conn_desc->rcv_buf);
	tcp_snd_queue_free(tcp_conn_desc->snd_buf);
	free_queue(tcp_conn_desc->back_log_i_queue);
	free_queue(tcp_conn_desc->back_log_c_queue);
	free_queue(tcp_conn_desc->rcv_queue);
	free_timer(tcp_conn_desc->rtrsn_timer);
	free_timer(tcp_conn_desc->pgybg_timer);
	kfree(tcp_conn_desc);
}

t_tcp_desc* tcp_init()
{
	t_tcp_desc* tcp_desc;
	
	tcp_desc = kmalloc(sizeof(t_tcp_desc));
	tcp_desc->conn_map = tcp_conn_map_init();
	tcp_desc->listen_map = tcp_conn_map_init();
	tcp_desc->req_map = tcp_conn_map_init();
	tcp_desc->tcp_conn_list = new_dllist();
	return tcp_desc;
}

void tcp_free(t_tcp_desc* tcp_desc)
{
	tcp_conn_map_free(tcp_desc->conn_map);
	tcp_conn_map_free(tcp_desc->listen_map);
	tcp_conn_map_free(tcp_desc->req_map);
	free_llist(tcp_desc->tcp_conn_list);
	kfree(tcp_desc);
}

static void update_rcv_window_and_ack(t_tcp_rcv_queue* tcp_queue)
{
	u32 ack_seq_num;
	u32 min;
	u32 offset;
	u32 index;
	u32 state_index;
	u32 wnd_max;

	offset = 0;
	index = tcp_queue->wnd_min;
	wnd_max = index + tcp_queue->wnd_size;
	while(index <= wnd_max)
	{
		state_index = SLOT_WND(index,(tcp_queue->buf_size / 8) + 1);
		slot_state = bit_vector_get(tcp_queue->buf_state,state_index);
		if (slot_state != 0)
		{
			break;
		}
		index++;
	}
	tcp_queue->nxt_rcv = index;
}

void rcv_packet_tcp(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len)
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
	u32 wnd_max;
	u32 data_len;
	u32 len_1;
	u32 len_2;
	t_tcp_conn_desc tcp_lstn_conn_desc = NULL;

	tcp_desc = system.network_desc->tcp_desc;
	tcp_row_packet = data_sckt_buf->transport_hdr;
	ip_row_packet = data_sckt_buf->network_hdr;
	src_port = GET_WORD(tcp_row_packet[0],tcp_row_packet[1]);
	dst_port = GET_WORD(tcp_row_packet[2],tcp_row_packet[3]);
	ack_seq_num = GET_DWORD(tcp_row_packet[8],tcp_row_packet[9],tcp_row_packet[10]tcp_row_packet[11]);
	seq_num = GET_DWORD(tcp_row_packet[4],tcp_row_packet[5],tcp_row_packet[6],tcp_row_packet[7]);
	flags = tcp_row_packet[14];

	tcp_req_desc = tcp_conn_map_get(tcp_desc->tcp_req_map,system.network_desc->ip,dst_ip,dst_port,src_port);
	tcp_listen_desc = tcp_conn_map_get(tcp_desc->tcp_listen_map,system.network_desc->ip,dst_port,0,0);
	if (tcp_req_desc == NULL && tcp_listen_desc == NULL)
	{ 
		goto EXIT;
	}

	//THREE WAY HANDSHAKE SYN + ACK FROM SERVER TO CLIENT
	if (flags & (FLG_SYN | FLG_ACK) && tcp_req_desc != NULL)
	{
		sono client qui
		if (tcp_req_desc != NULL)
		{
			tcp_req_desc->seq_num++;
			ack_num = seq_num + 1;
			send_packet_tcp(tcp_req_desc,NULL,0,ack_num,FLG_SYN | FLG_ACK);
			tcp_conn_map_remove(tcp_desc->tcp_req_map,src_ip,dst_ip,src_port,dst_port);
			tcp_conn_map_put(tcp_desc->tcp_conn_map,src_ip,dst_ip,src_port,dst_port,tcp_req_desc);
			ll_append(tcp_desc->tcp_conn_list,tcp_req_desc);
			tcp_req_desc->status = ESTABILISHED;
		}
		goto EXIT;
	}

	//THREE WAY HANDSHAKE SYN FROM CLIENT TO SERVER
	else if (flags & FLG_SYN && tcp_listen_desc != NULL)
	{
		tcp_conn_desc = tcp_conn_map_get(tcp_listen_desc->back_log_i_map,src_ip,dst_ip,src_port,dst_port);
		if (tcp_conn_desc == NULL)
		{
			tcp_conn_desc = tcp_conn_desc_int();
			tcp_conn_desc->src_ip = src_ip;
			tcp_conn_desc->dst_ip = dst_ip;
			tcp_conn_desc->src_port = src_port;
			tcp_conn_desc->dst_port = dst_port;
			tcp_conn_map_put(tcp_desc->back_log_i_map,src_ip,dst_ip,src_port,dst_port,tcp_conn_desc);
			ack_num = seq_num + 1;
			tcp_conn_desc->seq_num++;
			tcp_conn_desc->status = SYN_RCVD;
		}
		//COULD BE A LOST SYNC
		//THREE WAY HANDSHAKE SYN + ACK FROM SERVER TO CLIENT
		send_packet_tcp(tcp_conn_desc,NULL,0,ack_num,FLG_SYN | FLG_ACK);
		goto EXIT;
	}

	//THREE WAY HANDSHAKE ACK FROM CLIENT TO SERVER
	//IF THERE IS AN ACK COULD BE LAST STEP OF THREE WAY HANDSHAKE OR REGULAR PACKET
	else if (flags & FLG_ACK && tcp_listen_desc != NULL)
	{
		tcp_conn_desc = tcp_conn_map_get(tcp_listen_desc->back_log_i_queue,src_ip,dst_ip,src_port,dst_port);
		if (tcp_conn_desc != NULL)
		{
			if (tcp_conn_desc->seq_num +1 == ack_seq_num)
			{
				tcp_conn_map_remove(tcp_listen_desc->back_log_i_map,src_ip,dst_ip,src_port,dst_port);
				tcp_conn_map_put(tcp_listen_desc->back_log_c_map,src_ip,dst_ip,src_port,dst_port,tcp_conn_desc);
				tcp_conn_map_put(tcp_desc->tcp_conn_map,src_ip,dst_ip,src_port,dst_port,tcp_conn_desc);
				tcp_conn_desc->status = ESTABILISHED;
				//tcp_req_desc->rtrsn_timer->ref = ll_append(system.timers,tcp_req_desc->rtrsn_timer);
				//tcp_req_desc->pgybg_timer->ref = ll_append(system.timers,tcp_req_desc->pgybg_timer);
			}
			goto EXIT;
		}
	}
	tcp_conn_desc = tcp_conn_map_get(tcp_desc->tcp_conn_map,src_ip,dst_ip,src_port,dst_port);
	if (tcp_conn_desc == NULL) 
	{
		goto EXIT;
	}
	
	t_tcp_rcv_queue* tcp_queue=tcp_conn_desc->rcv_buf;
	ip_len=GET_WORD(ip_row_packet[2],ip_row_packet[3]);
	data_len=ip_len-HEADER_TCP;

	if (checksum_udp((unsigned short*) tcp_row_packet,src_ip,dst_ip,data_len)==0)
	{
		//FIN REQUEST BOTH FROM SERVER AND CLIENT
		if (flags & FLG_FIN && (tcp_conn_desc->status == FIN_WAIT_2 || tcp_conn_desc->status == FIN_WAIT_2 == ESTABILISHED ))
		{
			if (tcp_conn_desc->status == FIN_WAIT_2 )
			{
				//SHOULD BE CLOSE_WAIT AND SHOULD BE MANAGED 2MLS TIMER (TCP ILLUSTRATED PAG 590)
				tcp_conn_desc->status = CLOSE;
				send ack 
				release conn--------------qui
			}
			else if (tcp_conn_desc->status == ESTABILISHED )
			{
				tcp_conn_desc->status = TME_WAIT;
			}
			index = SLOT_WND(seq_num,tcp_queue->buf_size);
			kmemcpy(tcp_queue->buf + index,EOF,1);
			goto EXIT;
		}
		if (tcp_conn_desc->fin_seq_num == seq_num && tcp_conn_desc->status == CLOSED)
		{
			//stop timer;
			//tcp_conn_desc_free(tcp_conn_desc);
			goto EXIT;
		}
		wnd_max = tcp_queue->wnd_min + tcp_queue->wnd_size;
		if (seq_num >= tcp_queue->wnd_min && seq_num + data_len <= wnd_max)
		{
			low_index = SLOT_WND(seq_num,tcp_queue->buf_size);
			hi_index = SLOT_WND(seq_num + data_len,tcp_queue->buf_size);

			if (low_index < hi_index) 
			{
				kmemcpy(tcp_queue->buf + low_index,data_sckt_buf->data,data_len);
				for (i = low_index;i <= hi_index;i++)
				{
					slot_state = bit_vector_get(tcp_queue->buf_state,state_index);
					if (slot_state == 0)
					{
						bit_vector_set(tcp_queue->buf_state,state_index);
						tcp_queue->wnd_size--;
					}
				}
			}
			else 
			{
				len_1 = tcp_queue->size - low_index;
				len_2 = data_len - len_1;
				kmemcpy(tcp_queue->buf + low_index,data_sckt_buf->data,len_1);
				kmemcpy(tcp_queue->buf,data_sckt_buf->data+len_1,len_2);

				for (i = low_index;i <= len_1;i++)
				{
					slot_state = bit_vector_get(tcp_queue->buf_state,state_index);
					if (slot_state == 0)
					{
						bit_vector_set(tcp_queue->buf_state,state_index);
						tcp_queue->wnd_size--;
					}	
				}
				for (i = 0;i <= len_2;i++)
				{
					slot_state = bit_vector_get(tcp_queue->buf_state,state_index);
					if (slot_state == 0)
					{
						bit_vector_set(tcp_queue->buf_state,state_index);
						tcp_queue->wnd_size--;
					}
				}
			}
		}
		update_rcv_window_and_ack(tcp_queue);
		rcv_ack(tcp_conn_desc,ack_seq_num);
		update_snd_window(tcp_conn_desc,ack_seq_num,data_len);
		current_process_context = dequeue(tcp_conn_desc->queue);
		if (current_process_context != NULL)
		{
			_awake(current_process_context);
		}
	}
	EXIT:
		free_sckt(data_sckt_buf);
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
	if (tcp_conn_desc->min <= ack_seq_num)
	{
		if (tcp_conn_desc->duplicated_ack >0)
		{
			tcp_conn_desc->duplicated_ack=0;
			tcp_conn_desc->cwnd=tcp_conn_desc->ssthresh;
		}
		if (tcp_conn_desc->cwnd <= tcp_conn_desc->ssthresh)
		{
			tcp_conn_desc->cwnd +=SMSS;
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
}

static void update_snd_window(t_tcp_conn_desc* tcp_conn_desc,u32 ack_seq_num,u32 ack_data_len)
{
	u32 wnd_max;
	u32 word_to_ack;
	u32 expected_ack;
	u32 indx;
	t_tcp_snd_queue* tcp_queue = NULL;
	u8 flags=0;

	tcp_queue = tcp_conn_desc->snd_queue;
	ack_num = tcp_conn_desc->rcv_queue->nxt_rcv;

	// FIN flag retry
	if (tcp_conn_desc->status == CLOSED && tcp_conn_desc->fin_seq_num == ack_seq_num)
	{
		tcp_conn_desc->seq_num = tcp_conn_desc->fin_seq_num;
		send_packet_tcp(tcp_conn_desc,NULL,0,ack_num,FLG_FIN);
		return;
	}

	//close connection with FIN flag both client and server	
	else if (tcp_queue->buf_min == tcp_queue->buf_cur && (tcp_conn_desc->status == ESTABILISHED || tcp_conn_desc->status == CLOSE_WAIT ))
	{
		tcp_conn_desc->seq_num++;
		tcp_conn_desc->fin_seq_num = tcp_conn_desc->seq_num;
		send_packet_tcp(tcp_conn_desc,NULL,0,ack_num,FLG_FIN);
		return;
	}
	
	//trasmission with good ack
	else if (tcp_conn_desc->duplicated_ack == 0)
	{
		word_to_ack = ack_seq_num - tcp_queue->min;
		tcp_queue->wnd_min = tcp_queue->wnd_min + word_to_ack;
		wnd_max = tcp_queue->wnd_min + tcp_conn_desc->cwnd;
		data_to_send = wnd_max - tcp_queue->nxt_snd - 1;
		tcp_queue->buf_min = tcp_queue->wnd_min;

		//no data to send
		if (tcp_queue->wnd_min == tcp_queue->buf_cur)
		{
			ll_delete_node(tcp_conn_desc->rtrsn_timer->ref);	
			if (tcp_conn_desc->pgybg_timer == 0xFFFFFFFF && ack_num > 0)
			{
				tcp_conn_desc->pgybg_timer = PIGGYBACKING_TIMEOUT;
			}
			return;
		}
		else 
		{
			tcp_conn_desc->rtrsn_timer->val = tcp_conn_desc->rto;
		}

		if (tcp_queue->buf_cur >= tcp_queue->wnd_min && tcp_queue->buf_cur <= wnd_max)
		{	
			//in window
			//occorre inizializzare seq_num!!!!!!!!!!!!!!!!!!
			wnd_l_limit = tcp_snd_queue->nxt_snd;
			wnd_r_limit = tcp_queue->buf_cur;
		}
		if (tcp_queue->buf_cur >= wnd_max)
		{	
			wnd_l_limit = tcp_snd_queue->nxt_snd;
			wnd_r_limit = tcp_queue->buf_max;
		}
		
		//sender silly window avoidance		
		w_size = wnd_r_limit - wnd_l_limit;
		expected_ack = tcp_queue->nxt_snd - tcp_queue->data->wnd_min;
		if (expected_ack == 0)
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
		indx = SLOT_WND(wnd_l_limit,tcp_queue->size);
		tcp_desc->seq_num = tcp_queue->nxt_snd;
		tcp_queue->nxt_snd += data_to_send;
	}
	else if (tcp_conn_desc->duplicated_ack == 1 || tcp_conn_desc->duplicated_ack == 2)
	{
		wnd_max = tcp_queue->wnd_min + tcp_queue->wnd_size;
		w_size = wnd_max - tcp_snd_queue->nxt_snd;
		flight_size = tcp_snd_queue->nxt_snd-1 - tcp_snd_queue->wnd_min;
		flight_size_limit = tcp_conn_desc->cwnd + 2*SMSS;
		
		if (w_size >= SMSS && (flight_size + SMSS <= flight_size_limit) && tcp_queue->buf_cur >= (tcp_snd_queue->nxt_snd + SMSS))
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
		data_to_send = ack_data_len;
		tcp_snd_queue->nxt_snd += ack_data_len;
	}
	else if (tcp_conn_desc->duplicated_ack > 3)
	{
		indx = SLOT_WND(tcp_snd_queue->nxt_snd,tcp_queue->size);
		w_size = tcp_queue->max - tcp_snd_queue->nxt_snd;

		if (w_size >= SMSS && tcp_queue->buf_cur >= (tcp_snd_queue->nxt_snd + SMSS))
		{
			tcp_desc->seq_num = tcp_queue->nxt_snd;
			tcp_snd_queue->nxt_snd += SMSS;
			data_to_send = SMSS;
		}
	}

	if (data_to_send > 0)
	{
		if (if (ack_num > 0)
		{			
			//se ack == fin seq rimanda fin
			
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
		flags = 0;
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
		if (tcp_conn_desc->rtrsn_timer->val == 0)
		{
			//Al momento non ci sono problemi perche' sto con int disabilitati (da gestire caso con softirq)
			tcp_conn_desc->rtrsn_timer->val == tcp_conn_desc->rto;//aggiungere implenetazione rto
			tcp_req_desc->rtrsn_timer->ref = ll_append(system.timers,tcp_req_desc->rtrsn_timer);
		}
	}
}

void rtrsn_timer_handler(void* arg)
{
	t_tcp_conn_desc* tcp_conn_desc = NULL;

	tcp_conn_desc = (t_tcp_conn_desc*) arg;
	if (tcp_conn_desc->status == ESTABILISHED)
	{
		tcp_conn_desc->rto *= 2;
		tcp_conn_desc->rtrsn_timer->val = tcp_conn_desc->rto;
		tcp_conn_desc->cwnd = SMSS;
		tcp_conn_desc->snd_queue->nxt_snd = tcp_queue->wnd_min;
	}
	else if (tcp_conn_desc->status == ACTIVE_OPEN)
	{

	}
	else if (tcp_conn_desc->status == PASSIVE_OPEN)
	{

	}

	//to do!!!------------------------------------------------------qui!!!!!
	new_rto=a*rto + (1-a)rto_sample
	retrasmission sync????
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

int send_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len,u32 ack_num,u8 flags)
{
	u16 chk;
	char* tcp_payload = NULL;
	t_data_sckt_buf* data_sckt_buf = NULL;
	int ret = NULL;
	char* tcp_header = NULL;
	
	data_sckt_buf = alloc_sckt(data_len + HEADER_ETH + HEADER_IP4 + HEADER_TCP);
	data_sckt_buf->transport_hdr = data_sckt_buf->data + HEADER_ETH + HEADER_IP4;
	tcp_payload = data_sckt_buf->transport_hdr + HEADER_TCP;
	kmemcpy(tcp_payload,data,data_len);
	tcp_header=data_sckt_buf->transport_hdr;
	
	tcp_header[0] = HI_16(tcp_conn_desc->src_port);   			//HI SRC PORT
	tcp_header[1] = LOW_16(tcp_conn_desc->src_port);  			//LOW SRC PORT
	tcp_header[2] = HI_16(tcp_conn_desc->dst_port);   			//HI DST PORT
	tcp_header[3] = LOW_16(tcp_conn_desc->dst_port);  			//LOW DST PORT

	tcp_header[4] = HI_OCT_32(tcp_conn_desc->seq_num);         	//HIGH SEQ NUM
	tcp_header[5] = MID_LFT_OCT_32(tcp_conn_desc->seq_num);    	//MID LEFT SEQ NUM
	tcp_header[6] = MID_RGT_OCT_32(tcp_conn_desc->seq_num);    	//MID RIGHT SEQ NUM
	tcp_header[7] = LOW_OCT_32(tcp_conn_desc->seq_num);        	//LOW SEQ NUM

	tcp_header[8] = HI_OCT_32(ack_num);							//HIGH ACK NUM
	tcp_header[9] = MID_LFT_OCT_32(ack_num);					//MID LEFT ACK NUM
	tcp_header[10] = MID_RGT_OCT_32(ack_num); 					//MID RIGHT ACK NUM
	tcp_header[11] = LOW_OCT_32(ack_num);						//LOW ACK NUM

	tcp_header[12] = 0x50;	                   					//HEADER LEN + 4 RESERVED BIT (5 << 4)
	tcp_header[13] = flags;                           			//FLAGS
	tcp_header[14] = HI_16(tcp_conn_desc->wnd_size);			//HI WINDOW SIZE
	tcp_header[15] = LOW_16(tcp_conn_desc->wnd_size);           //LOW WINDOW SIZE

	tcp_header[16] = HI_16(checksum);							//HI TCP CHECKSUM
	tcp_header[17] = LOW_16(checksum);							//LOW TCP CHECKSUM
	tcp_header[18] = 0;											//HI URGENT POINTER (NOT USED)
	tcp_header[19] = 0;											//LOW URGENT POINTER (NOT USED)

	chk = SWAP_WORD(checksum_tcp((unsigned short*) tcp_header,tcp_conn_desc->src_ip,tcp_conn_desc->dst_ip,data_len));
	tcp_header[18] = HI_16(chk);
	tcp_header[19] = LOW_16(chk);

	ret = send_packet_ip4(data_sckt_buf,
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
