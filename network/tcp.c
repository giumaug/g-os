//BASED ON RFC5681 AND RFC6298
#include "common.h"
#include "network/tcp.h"
#include "lib/lib.h"
#include "debug.h"

extern unsigned int collect_mem;
static void rcv_ack(t_tcp_conn_desc* tcp_conn_desc,u32 ack_seq_num,u32 data_len);
static u16 checksum_tcp(char* tcp_row_packet,u32 src_ip,u32 dst_ip,u16 data_len,u8 tcp_header_len);
static void flush_data(t_tcp_conn_desc* tcp_conn_desc,u32 data_to_send,u32 ack_num,u32 indx);
void delay();

static void upd_max_adv_wnd(t_tcp_conn_desc* tcp_conn_desc,u32 rcv_wmd_adv)
{
	tcp_conn_desc->rcv_wmd_adv = rcv_wmd_adv;
	if (tcp_conn_desc->max_adv_wnd < rcv_wmd_adv)
	{
		tcp_conn_desc->max_adv_wnd = rcv_wmd_adv;
	}
}

static t_tcp_snd_queue* tcp_snd_queue_init(u32 size)
{
	t_tcp_snd_queue* tcp_snd_queue = NULL;

	tcp_snd_queue = kmalloc(sizeof(t_tcp_snd_queue));
	tcp_snd_queue->buf = buddy_alloc_page(system.buddy_desc,TCP_SND_SIZE);
	tcp_snd_queue->wnd_min = 4293967296; //1
	tcp_snd_queue->wnd_size = SMSS;
	tcp_snd_queue->cur = 1;
	tcp_snd_queue->buf_size = TCP_SND_SIZE;
	//to inizialize to random seq_num
	tcp_snd_queue->nxt_snd = 4293967296; //1
	tcp_snd_queue->pnd_data = 0;
	return tcp_snd_queue;
}

static void tcp_snd_queue_free(t_tcp_snd_queue* tcp_snd_queue)
{
	buddy_free_page(system.buddy_desc,tcp_snd_queue->buf);
	kfree(tcp_snd_queue);
}

static t_tcp_rcv_queue* tcp_rcv_queue_init(u32 size)
{
	t_tcp_rcv_queue* tcp_rcv_queue;

	tcp_rcv_queue = kmalloc(sizeof(t_tcp_rcv_queue));
	tcp_rcv_queue->buf = kmalloc(TCP_RCV_SIZE);
	tcp_rcv_queue->buf_state = kmalloc(TCP_RCV_SIZE);
	tcp_rcv_queue->wnd_min = 0;
	tcp_rcv_queue->wnd_size = TCP_RCV_SIZE;
	tcp_rcv_queue->buf_size = TCP_RCV_SIZE;
	tcp_rcv_queue->last_adv_wnd = 0;
	tcp_rcv_queue->nxt_rcv = 0; 
	tcp_rcv_queue->seq_num = 0;
	tcp_rcv_queue->data_len = 0;
	tcp_rcv_queue->is_wnd_hole = 0;
	tcp_rcv_queue->max_seq_num = 0;
	return tcp_rcv_queue;
}

static void tcp_rcv_queue_free(t_tcp_rcv_queue* tcp_rcv_queue)
{
	kfree(tcp_rcv_queue->buf_state);
	kfree(tcp_rcv_queue->buf);
	kfree(tcp_rcv_queue);
}

void reset(t_tcp_conn_desc* tcp_conn_desc)
{
	bit_vector_re_init(tcp_conn_desc->rcv_queue->buf_state,TCP_RCV_SIZE);
	if (tcp_conn_desc->rtrsn_timer->ref != NULL)
	{
  		ll_delete_node(tcp_conn_desc->rtrsn_timer->ref);
		tcp_conn_desc->rtrsn_timer->ref = NULL;
	}
	if (tcp_conn_desc->pgybg_timer->ref != NULL)
	{
  		ll_delete_node(tcp_conn_desc->pgybg_timer->ref);
		tcp_conn_desc->pgybg_timer->ref = NULL;
	}

	tcp_conn_desc->rcv_queue->wnd_min = 0;
	tcp_conn_desc->rcv_queue->wnd_size = TCP_RCV_SIZE;
	tcp_conn_desc->rcv_queue->buf_size = TCP_RCV_SIZE;
	tcp_conn_desc->rcv_queue->nxt_rcv = 0; 

	tcp_conn_desc->rcv_queue->seq_num = 0;
	tcp_conn_desc->rcv_queue->data_len = 0;

	tcp_conn_desc->snd_queue->wnd_min = 1;
	tcp_conn_desc->snd_queue->wnd_size = SMSS;
	tcp_conn_desc->snd_queue->cur = 1;
	tcp_conn_desc->snd_queue->buf_size = TCP_SND_SIZE;
	tcp_conn_desc->snd_queue->nxt_snd = 1;
}

t_tcp_conn_desc* tcp_conn_desc_int()
{
 	t_tcp_conn_desc* tcp_conn_desc = NULL;
	t_tcp_desc* tcp_desc = system.network_desc->tcp_desc;
	
	tcp_conn_desc = kmalloc(sizeof(t_tcp_conn_desc));
	tcp_conn_desc->rcv_queue = tcp_rcv_queue_init(TCP_RCV_SIZE);
	tcp_conn_desc->snd_queue = tcp_snd_queue_init(TCP_SND_SIZE);
	tcp_conn_desc->rtrsn_timer = timer_init(0,&rtrsn_timer_handler,tcp_conn_desc,NULL);
	tcp_conn_desc->pgybg_timer = timer_init(0,&pgybg_timer_handler,tcp_conn_desc,NULL);
	tcp_conn_desc->back_log_i_map = tcp_conn_map_init();
	tcp_conn_desc->back_log_c_queue = new_queue(&tcp_conn_desc_free);
	tcp_conn_desc->rto = DEFAULT_RTO;
	tcp_conn_desc->srtt = DEFAULT_RTO;
	tcp_conn_desc->cwnd = SMSS;
	tcp_conn_desc->ssthresh = WND_ADV;
	tcp_conn_desc->rcv_wmd_adv = 0;
	tcp_conn_desc->max_adv_wnd = 0;
	tcp_conn_desc->src_ip = 0;
	tcp_conn_desc->dst_ip = 0;
	tcp_conn_desc->src_port = 0;
	tcp_conn_desc->dst_port = 0;
	SPINLOCK_INIT(tcp_conn_desc->lock);
	tcp_conn_desc->status = CLOSED;
	tcp_conn_desc->duplicated_ack = 0;
	tcp_conn_desc->last_sent_time = 0;
	tcp_conn_desc->flight_size = 0;
	tcp_conn_desc->ref_count = 1;
	tcp_conn_desc->last_sent_time = 0;
	tcp_conn_desc->isActive = 0;
	tcp_conn_desc->process_context = NULL;
	tcp_conn_desc->pending_ack = 0;
	return tcp_conn_desc;
}

void tcp_conn_desc_free(t_tcp_conn_desc* tcp_conn_desc)
{
	t_tcp_desc* tcp_desc = system.network_desc->tcp_desc;

	tcp_rcv_queue_free(tcp_conn_desc->rcv_queue);
	tcp_snd_queue_free(tcp_conn_desc->snd_queue);
	tcp_conn_map_free(tcp_conn_desc->back_log_i_map);
	free_queue(tcp_conn_desc->back_log_c_queue);
	timer_free(tcp_conn_desc->rtrsn_timer);
	timer_free(tcp_conn_desc->pgybg_timer);
	if (tcp_conn_desc->isActive)
	{
		hashtable_remove(system.network_desc->tcp_desc->ep_port_map,tcp_conn_desc->src_port);
	}
	kfree(tcp_conn_desc);
}

t_tcp_desc* tcp_init()
{
	t_tcp_desc* tcp_desc;
	
	tcp_desc = kmalloc(sizeof(t_tcp_desc));
	tcp_desc->conn_map = tcp_conn_map_init();
	tcp_desc->listen_map = tcp_conn_map_init();
	tcp_desc->req_map = tcp_conn_map_init();
	tcp_desc->ep_port_map = hashtable_init(EPHEMERAL_PORT_MAP_SIZE);
	tcp_desc->ep_port_index = 32767;
	return tcp_desc;
}

void tcp_free(t_tcp_desc* tcp_desc)
{
	tcp_conn_map_free(tcp_desc->conn_map);
	tcp_conn_map_free(tcp_desc->listen_map);
	tcp_conn_map_free(tcp_desc->req_map);
	hashtable_free(tcp_desc->ep_port_map);
	kfree(tcp_desc);
}

static u8 _update_rcv_window_and_ack(t_tcp_rcv_queue* tcp_queue,int data_len)
{
	int i;
	u32 ack_seq_num;
	u32 min;
	u32 offset;
	unsigned long long index;
	
	u32 state_index;
	unsigned long long wnd_max;
	u8 slot_state;
	u8 ret;

	ret = 0;
	index = tcp_queue->wnd_min;
	wnd_max = index + (long long)tcp_queue->buf_size;
	
	//Second condition in order to manage buffer overflow.
	while(index < wnd_max && index < tcp_queue->max_seq_num)
	{
		state_index = SLOT_WND((u32)index,tcp_queue->buf_size);
		//state_index = index & TCP_RCV_SIZE;
		if (tcp_queue->buf_state[state_index] == 0)
		{
			break;
		}
		index++;
	}
	tcp_queue->nxt_rcv = index;
	if (tcp_queue->nxt_rcv > tcp_queue->wnd_min)
	{
		ret = 1;
		if (index < tcp_queue->max_seq_num)
		{
			panic();
		}
	}
	if (tcp_queue->nxt_rcv == tcp_queue->max_seq_num)
	{
		tcp_queue->is_wnd_hole = 0;
		for (i = 0; i <= TCP_RCV_SIZE;i++)
		{
			tcp_queue->buf_state[i]	= 0;
		}
		tcp_queue->max_seq_num = 0;
	}
	return ret;
}

static u8 update_rcv_window_and_ack(t_tcp_rcv_queue* tcp_queue,int data_len,u32 seq_num)
{
	u8 ret = 1;

	if (tcp_queue->nxt_rcv == seq_num && tcp_queue->is_wnd_hole == 0)
	{
		tcp_queue->nxt_rcv += data_len;
		tcp_queue->wnd_size -= data_len;
	}
	else
	{
		ret = _update_rcv_window_and_ack(tcp_queue,data_len);
	}
	return ret;
}

void rcv_packet_tcp(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len)
{
	unsigned int diff;
	t_tcp_desc* tcp_desc = NULL;
	t_tcp_conn_desc* tcp_conn_desc = NULL;
	unsigned char* tcp_row_packet = NULL;
	unsigned char* ip_row_packet = NULL;
	u16 src_port;
	u16 dst_port;
	u32 conn_id;
	u32 seq_num;
	u32 ack_seq_num;
	u32 ack_num;
	long long wnd_max;
	u32 rcv_wmd_adv;
	u32 len_1;
	u32 len_2;
	u32 index;
	u32 low_index;
	u32 hi_index;
	u8 slot_state;
	u32 i;
	t_tcp_conn_desc* tcp_lstn_conn_desc = NULL;
	t_tcp_conn_desc* tcp_req_desc = NULL;
	t_tcp_conn_desc* tcp_listen_desc = NULL;
	t_tcp_conn_desc* new_tcp_conn_desc = NULL;
	t_tcp_conn_desc* pending_tcp_conn_desc = NULL;
	struct t_process_context* process_context = NULL;
	u8 flags;
	u8 is_new_data;
	u8 free_conn;
	u8 no_piggy = 0;
	u32 adv_wnd_size = 0;
	static counter = 0;
	static miss_seq_num = 0;
	
	#ifdef PROFILE
	long long time_1;
	long long time_2;
	rdtscl(&time_1);
	#endif
	
	free_conn = 0;
	tcp_desc = system.network_desc->tcp_desc;
	tcp_row_packet = data_sckt_buf->transport_hdr;
	ip_row_packet = data_sckt_buf->network_hdr;
	src_port = GET_WORD(tcp_row_packet[0],tcp_row_packet[1]);
	dst_port = GET_WORD(tcp_row_packet[2],tcp_row_packet[3]);
	ack_seq_num = GET_DWORD(tcp_row_packet[8],tcp_row_packet[9],tcp_row_packet[10],tcp_row_packet[11]);
	seq_num = GET_DWORD(tcp_row_packet[4],tcp_row_packet[5],tcp_row_packet[6],tcp_row_packet[7]);
	flags = tcp_row_packet[13];
	rcv_wmd_adv = GET_WORD(tcp_row_packet[14],tcp_row_packet[15]);
	
	if (checksum_tcp((unsigned short*) tcp_row_packet,src_ip,dst_ip,data_len,HEADER_TCP) !=0 )
	{
		printk("wrong checksum \n");
		goto EXIT;
	}
	tcp_conn_desc = tcp_conn_map_get(tcp_desc->conn_map,dst_ip,src_ip,dst_port,src_port);
	tcp_req_desc = tcp_conn_map_get(tcp_desc->req_map,dst_ip,system.network_desc->ip,dst_port,src_port);
	tcp_listen_desc = tcp_conn_map_get(tcp_desc->listen_map,system.network_desc->ip,0,dst_port,0);

	if (tcp_req_desc == NULL && tcp_listen_desc == NULL && tcp_conn_desc == NULL )
	{
		printk("no connection !!!!!! \n"); 
		goto EXIT;
	}
	if (flags & 4)
	{
		if (tcp_conn_desc != NULL)
		{
			timer_reset(tcp_conn_desc->rtrsn_timer);
			tcp_conn_desc->status = RESET;
			if (tcp_conn_desc->ref_count == 0)
			{
				tcp_conn_map_remove(tcp_desc->conn_map,
				            	    tcp_conn_desc->src_ip,
				                    tcp_conn_desc->dst_ip,
				                    tcp_conn_desc->src_port,
				                    tcp_conn_desc->dst_port);
				tcp_conn_desc_free(tcp_conn_desc);
				goto EXIT;
			}
		}
		else if (tcp_req_desc != NULL)
		{
			timer_reset(tcp_req_desc->rtrsn_timer);
			tcp_conn_desc->status = RESET;
		}
		else if (tcp_listen_desc != NULL)
		{
			pending_tcp_conn_desc = tcp_conn_map_get(tcp_listen_desc->back_log_i_map,dst_ip,src_ip,dst_port,src_port);
			if (pending_tcp_conn_desc != NULL)
			{
				tcp_conn_map_remove(tcp_listen_desc->back_log_i_map,dst_ip,src_ip,dst_port,src_port);
				tcp_conn_desc_free(pending_tcp_conn_desc);
			}
		}
	}
	
	//THREE WAY HANDSHAKE SYN + ACK FROM SERVER TO CLIENT
	if ((flags & FLG_SYN) && (flags & FLG_ACK) && (tcp_req_desc != NULL || tcp_conn_desc !=NULL))
	{
		//FIRST TIME
		if (tcp_req_desc != NULL)
		{
			ack_num = seq_num + 1;
			tcp_req_desc->rcv_queue->nxt_rcv = ack_num;

//			sort();
//			for (i = 0; i< 140000;i++)
//			{
//				foo();
//			}

			_SEND_PACKET_TCP(tcp_req_desc,NULL,0,ack_num,FLG_ACK,++tcp_req_desc->snd_queue->nxt_snd);
			tcp_conn_map_remove(tcp_desc->req_map,dst_ip,src_ip,dst_port,src_port);
			tcp_conn_map_put(tcp_desc->conn_map,dst_ip,src_ip,dst_port,src_port,tcp_req_desc);
			tcp_req_desc->status = ESTABILISHED;
			tcp_req_desc->snd_queue->cur =  tcp_req_desc->snd_queue->nxt_snd;
			upd_max_adv_wnd(tcp_req_desc,rcv_wmd_adv);
			tcp_req_desc->rcv_queue->wnd_min = ack_num;
			tcp_req_desc->snd_queue->wnd_min = tcp_req_desc->snd_queue->nxt_snd;
			timer_reset(tcp_req_desc->rtrsn_timer);
			_awake(tcp_req_desc->process_context);
			tcp_req_desc->process_context = NULL;
		}
		//RETRY
		else if (tcp_conn_desc != NULL)
		{
			upd_max_adv_wnd(tcp_conn_desc,rcv_wmd_adv);
			_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,ack_num,FLG_ACK,tcp_conn_desc->snd_queue->nxt_snd);
			timer_reset(tcp_conn_desc->rtrsn_timer);
		}
		goto EXIT;
	}

	//THREE WAY HANDSHAKE SYN FROM CLIENT TO SERVER
	else if (flags & FLG_SYN && tcp_listen_desc != NULL)
	{
		ack_num = seq_num + 1;
		new_tcp_conn_desc = tcp_conn_map_get(tcp_listen_desc->back_log_i_map,dst_ip,src_ip,dst_port,src_port);
		if (new_tcp_conn_desc == NULL)
		{
			new_tcp_conn_desc = tcp_conn_desc_int();
			new_tcp_conn_desc->src_ip = dst_ip;
			new_tcp_conn_desc->dst_ip = src_ip;
			new_tcp_conn_desc->src_port = dst_port;
			new_tcp_conn_desc->dst_port = src_port;
			tcp_conn_map_put(tcp_listen_desc->back_log_i_map,dst_ip,src_ip,dst_port,src_port,new_tcp_conn_desc);
			
			new_tcp_conn_desc->snd_queue->nxt_snd++;
			new_tcp_conn_desc->status = SYN_RCVD;
			new_tcp_conn_desc->rcv_queue->nxt_rcv = ack_num;
			new_tcp_conn_desc->rcv_queue->wnd_min = ack_num;
			timer_set(new_tcp_conn_desc->rtrsn_timer,new_tcp_conn_desc->rto);
		}
		//Workaround to avoid new connection on still open port (issue with cell network) 
		else if (new_tcp_conn_desc->status != SYN_RCVD) {
			goto EXIT;
		}
		upd_max_adv_wnd(new_tcp_conn_desc,rcv_wmd_adv);
		_SEND_PACKET_TCP(new_tcp_conn_desc,NULL,0,ack_num,(FLG_SYN | FLG_ACK),new_tcp_conn_desc->snd_queue->nxt_snd);
		goto EXIT;
	}

	//THREE WAY HANDSHAKE ACK FROM CLIENT TO SERVER
	//IF THERE IS AN ACK COULD BE LAST STEP OF THREE WAY HANDSHAKE OR REGULAR PACKET
	else if (flags & FLG_ACK && tcp_listen_desc != NULL)
	{
		new_tcp_conn_desc = tcp_conn_map_get(tcp_listen_desc->back_log_i_map,dst_ip,src_ip,dst_port,src_port);
		if (new_tcp_conn_desc != NULL)
		{
			if (new_tcp_conn_desc->snd_queue->nxt_snd + 1 == ack_seq_num)
			{
				tcp_conn_map_remove(tcp_listen_desc->back_log_i_map,dst_ip,src_ip,dst_port,src_port);
				enqueue(tcp_listen_desc->back_log_c_queue,new_tcp_conn_desc);
				tcp_conn_map_put(tcp_desc->conn_map,dst_ip,src_ip,dst_port,src_port,new_tcp_conn_desc);
				new_tcp_conn_desc->status = ESTABILISHED;		
				new_tcp_conn_desc->snd_queue->nxt_snd++;
				new_tcp_conn_desc->snd_queue->cur =  new_tcp_conn_desc->snd_queue->nxt_snd;
				new_tcp_conn_desc->rcv_wmd_adv = rcv_wmd_adv;
				new_tcp_conn_desc->snd_queue->wnd_min = new_tcp_conn_desc->snd_queue->nxt_snd;
				upd_max_adv_wnd(new_tcp_conn_desc,rcv_wmd_adv);
				if (tcp_listen_desc->process_context !=NULL)
				{
					_awake(tcp_listen_desc->process_context);
					tcp_listen_desc->process_context = NULL;
				}
				timer_reset(new_tcp_conn_desc->rtrsn_timer);	
			}
			goto EXIT;
		}
	}
	tcp_conn_desc = tcp_conn_map_get(tcp_desc->conn_map,dst_ip,src_ip,dst_port,src_port);
	if (tcp_conn_desc == NULL) 
	{
		printk("no conn \n");
		goto EXIT;
	}

	//-----------
	if (system.conn == NULL)
	{
		system.conn = tcp_conn_desc;
	}
	//----------

	t_tcp_rcv_queue* tcp_queue = tcp_conn_desc->rcv_queue;
	upd_max_adv_wnd(tcp_conn_desc,rcv_wmd_adv);

//START ACTIVE CLOSE
//	FIN,ACK OR FIN AND ACK FROM SERVER
	u32 fin_num = tcp_conn_desc->snd_queue->nxt_snd;
	u32 ack_fin_num;
	if (data_len > 0)
	{
		ack_fin_num = seq_num + data_len + 1;
	}
	else
	{
		ack_fin_num = seq_num + 1;
	}
	if ((flags & FLG_FIN) && (flags & FLG_ACK) 
	    && tcp_conn_desc->status == FIN_WAIT_1)
	{
		if (fin_num + 1 == ack_seq_num)
		{
			//SHOULD BE CLOSE_WAIT AND SHOULD BE MANAGED 2MLS TIMER (TCP ILLUSTRATED PAG 590)
			tcp_conn_desc->status = CLOSED;
			_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,ack_fin_num,FLG_ACK,(fin_num + 1));
			tcp_conn_map_remove(tcp_desc->conn_map,
					    tcp_conn_desc->src_ip,
					    tcp_conn_desc->dst_ip,
					    tcp_conn_desc->src_port,
				            tcp_conn_desc->dst_port);

			tcp_conn_desc->snd_queue->nxt_snd = fin_num + 1;
			timer_reset(tcp_conn_desc->rtrsn_timer);
			free_conn = 1;
			no_piggy = 1;
		}
		else
		{
			//TO CHECK RETRY SU FIN WITH WRONG ACK
		}
	}
	else if (flags & FLG_ACK 
	    	 && tcp_conn_desc->status == FIN_WAIT_1
	    	 && (fin_num + 1) == ack_seq_num)
	{
		tcp_conn_desc->status = FIN_WAIT_2;
		timer_reset(tcp_conn_desc->rtrsn_timer);
		//goto EXIT;
	}
	else if (flags & FLG_FIN && tcp_conn_desc->status == FIN_WAIT_2)
	{
		//SHOULD BE CLOSE_WAIT AND SHOULD BE MANAGED 2MLS TIMER (TCP ILLUSTRATED PAG 590)
		tcp_conn_desc->status = CLOSED;
		_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,ack_fin_num,FLG_ACK,(fin_num + 1));
		tcp_conn_desc->snd_queue->nxt_snd = fin_num + 1;
		timer_reset(tcp_conn_desc->rtrsn_timer);
		tcp_conn_map_remove(tcp_desc->conn_map,tcp_conn_desc->src_ip,tcp_conn_desc->dst_ip,tcp_conn_desc->src_port,tcp_conn_desc->dst_port);
		free_conn = 1;
		no_piggy = 1;
	}
	else if (flags & FLG_FIN && tcp_conn_desc->status == FIN_WAIT_1)
	{
		//TO CHECK RETRY SU FIN WITH WRONG ACK
	}
//END ACTIVE CLOSE

//START PASSIVE CLOSE
//	FIN,ACK OR FIN AND ACK FROM CLIENT
	else if ((flags & FLG_FIN) 
	    && tcp_conn_desc->status == ESTABILISHED)
	{
		if (seq_num == tcp_conn_desc->rcv_queue->nxt_rcv)
		{
			tcp_conn_desc->status = CLOSE_WAIT;
		}
		else
		{
			ack_fin_num = tcp_conn_desc->rcv_queue->nxt_rcv;
		}
		
		_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,ack_fin_num,FLG_ACK,fin_num);
		tcp_conn_desc->snd_queue->nxt_snd = fin_num;
		timer_reset(tcp_conn_desc->rtrsn_timer);
		no_piggy = 1;
	}
	else if ((flags & FLG_ACK) 
	    && tcp_conn_desc->status == LAST_ACK
	    && (fin_num + 1) == ack_seq_num)
	{
		timer_reset(tcp_conn_desc->rtrsn_timer);
		tcp_conn_map_remove(tcp_desc->conn_map,tcp_conn_desc->src_ip,tcp_conn_desc->dst_ip,tcp_conn_desc->src_port,tcp_conn_desc->dst_port);
		free_conn = 1;
	}
//END PASSIVE CLOSE
	if (data_len != 0)
	{
		tcp_queue->seq_num = seq_num;
		tcp_queue->data_len = data_len;
		
		if (tcp_conn_desc->pgybg_timer->val == 0 && no_piggy == 0)
		{
			timer_set(tcp_conn_desc->pgybg_timer,PIGGYBACKING_TIMEOUT);
		}
		//wnd_max = tcp_queue->nxt_rcv + (long long) tcp_queue->wnd_size;
		wnd_max = tcp_queue->wnd_min + (long long) tcp_queue->buf_size;
		if (seq_num >= tcp_queue->nxt_rcv && seq_num + data_len <= wnd_max)
		{
			low_index = SLOT_WND(seq_num,tcp_queue->buf_size);
			hi_index = SLOT_WND((seq_num + data_len),tcp_queue->buf_size);
			if (low_index < hi_index) 
			{
				kmemcpy(tcp_queue->buf + low_index,data_sckt_buf->transport_hdr+HEADER_TCP,data_len);
				if (tcp_queue->nxt_rcv != seq_num || tcp_queue->is_wnd_hole == 1)
				{
					tcp_queue->is_wnd_hole = 1;
					if (tcp_queue->max_seq_num  < (seq_num + data_len))
					{
						tcp_queue->max_seq_num = seq_num + data_len;
					}
					for (i = low_index;i < hi_index;i++)
					{
						if (tcp_queue->buf_state[i] == 0)
						{
							tcp_queue->buf_state[i] = 1;
							tcp_queue->wnd_size--;
						}
					}
				}
			}
			else 
			{
	     			//gestire qui PIGGYBACKING_TIMEOUT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				len_1 = tcp_queue->buf_size - low_index;
				len_2 = data_len - len_1;
				kmemcpy(tcp_queue->buf + low_index,data_sckt_buf->transport_hdr+HEADER_TCP,len_1);
				kmemcpy(tcp_queue->buf,data_sckt_buf->transport_hdr+HEADER_TCP + len_1,len_2);
				if (tcp_queue->nxt_rcv != seq_num || tcp_queue->is_wnd_hole == 1)
				{
					tcp_queue->is_wnd_hole = 1;
					if (tcp_queue->max_seq_num  < (seq_num + data_len))
					{
						tcp_queue->max_seq_num = seq_num + data_len;
					}
					for (i = low_index ; i < (low_index + len_1) ; i++)
					{
						if (tcp_queue->buf_state[i] == 0)
						{
							tcp_queue->buf_state[i] = 1;
							tcp_queue->wnd_size--;
						}	
					}
					for (i = 0;i < len_2;i++)
					{
						if (tcp_queue->buf_state[i] == 0)
						{
							tcp_queue->buf_state[i] = 1;
							tcp_queue->wnd_size--;
						}
					}
				}
			}
			is_new_data = update_rcv_window_and_ack(tcp_queue,data_len,seq_num);
			if (tcp_conn_desc->process_context != NULL && is_new_data > 0)
			{
				_awake(tcp_conn_desc->process_context);
				tcp_conn_desc->process_context = NULL;
				if (tcp_queue->nxt_rcv - tcp_queue->wnd_min == 0)
				{
					panic();
				}
			}
			else
			{
				//COULD HAPPEN WITH OUT OF ORDER PACKET.PANIC NO NEEDED.			
				if(tcp_conn_desc->process_context != NULL && is_new_data == 0)
				{
					//panic();
				}
			}				
		}
		else
		{
			printk("window full!!! ");
			goto EXIT;
		}
//		is_new_data = update_rcv_window_and_ack(tcp_queue,data_len,seq_num);
//		if (tcp_conn_desc->process_context != NULL && is_new_data > 0)
//		{
//			_awake(tcp_conn_desc->process_context);
//			printk("a");
//			tcp_conn_desc->process_context = NULL;
//			if (tcp_queue->nxt_rcv - tcp_queue->wnd_min == 0)
//			{
//				panic();
//			}
//		}
//		else
//		{
//			//COULD HAPPEN WITH OUT OF ORDER PACKET.PANIC NO NEEDED.			
//			if(tcp_conn_desc->process_context != NULL && is_new_data == 0)
//			{
//				//panic();
//			}
//		}
	}
	//CHECK DUPLICATE ACK DEFINITION RFC5681
	if (CHK_OVRFLW(tcp_conn_desc->snd_queue->nxt_snd,tcp_conn_desc->snd_queue->wnd_min) - (long long)tcp_conn_desc->snd_queue->wnd_min > 0)
	{	
		rcv_ack(tcp_conn_desc,ack_seq_num,data_len);
	}
EXIT:
		if (tcp_conn_desc != NULL)
		{
			if (data_len != 0) 
			{
				tcp_conn_desc->pending_ack++;
				if (tcp_conn_desc->rcv_queue->wnd_size >= (tcp_conn_desc->rcv_queue->last_adv_wnd + SMSS) ||
				    tcp_conn_desc->rcv_queue->wnd_size >= (TCP_RCV_SIZE - SMSS))
				{  
					if (tcp_conn_desc->pending_ack >= 2)
					{
						tcp_conn_desc->pending_ack = 0;
						tcp_conn_desc->rcv_queue->last_adv_wnd = tcp_conn_desc->rcv_queue->wnd_size;

						timer_reset(tcp_conn_desc->pgybg_timer);
						send_packet_tcp(tcp_conn_desc->src_ip,
                        					tcp_conn_desc->dst_ip,
                        					tcp_conn_desc->src_port,
                        					tcp_conn_desc->dst_port,
								tcp_conn_desc->rcv_queue->wnd_size,
                        					NULL,
                        					0,
                        					tcp_conn_desc->rcv_queue->nxt_rcv,
                        					FLG_ACK,
                        					tcp_conn_desc->snd_queue->nxt_snd);  
					}
				}
			}
		}
		free_sckt(data_sckt_buf);
		if (free_conn)
		{
			tcp_conn_desc_free(tcp_conn_desc);
		}
		//printk("status= %d \n",tcp_conn_desc->status);
		//printk("fin_num= %d \n",fin_num);
		//printk("ack_seq_num= %d \n",ack_seq_num);
		//printk("seq_num= %d \n",seq_num);
		//printk("wnd %d ",tcp_queue->wnd_size);
		#ifdef PROFILE
		rdtscl(&time_2);
		system.tot_read_time += (time_2 - time_1);
		#endif
}

void update_adv_wnd(t_tcp_conn_desc* tcp_conn_desc)
{
	send_packet_tcp(tcp_conn_desc->src_ip,
                        tcp_conn_desc->dst_ip,
                        tcp_conn_desc->src_port,
                        tcp_conn_desc->dst_port,
			tcp_conn_desc->rcv_queue->wnd_size,
                        NULL,
                        0,
                        tcp_conn_desc->rcv_queue->nxt_rcv,
                        FLG_ACK,
                        tcp_conn_desc->snd_queue->nxt_snd); 
}
	
static void rcv_ack(t_tcp_conn_desc* tcp_conn_desc,u32 ack_seq_num,u32 data_len)
{
	t_tcp_snd_queue* tcp_queue = NULL;
	u32 rtt = 0;
	
	long long _ack_seq_num = CHK_OVRFLW(ack_seq_num,tcp_conn_desc->snd_queue->wnd_min);
	long long _nxt_snd = CHK_OVRFLW(tcp_conn_desc->snd_queue->nxt_snd,tcp_conn_desc->snd_queue->wnd_min);
	if ((long long)tcp_conn_desc->snd_queue->wnd_min <= _ack_seq_num)
	{
		if ((long long)tcp_conn_desc->snd_queue->wnd_min < _ack_seq_num)
		{
			if (tcp_conn_desc->snd_queue->nxt_snd == ack_seq_num)
			{
				timer_reset(tcp_conn_desc->rtrsn_timer);
			}
			else if (_nxt_snd > _ack_seq_num)
			{
				timer_set(tcp_conn_desc->rtrsn_timer,tcp_conn_desc->rto);
			}
			else if (_nxt_snd < _ack_seq_num)
			{
				timer_set(tcp_conn_desc->rtrsn_timer,tcp_conn_desc->rto);
				tcp_conn_desc->snd_queue->nxt_snd = ack_seq_num;
			}
			if (tcp_conn_desc->duplicated_ack > 0)
			{
				tcp_conn_desc->duplicated_ack = 0;
				tcp_conn_desc->cwnd = tcp_conn_desc->ssthresh;
			}
			if (tcp_conn_desc->cwnd <= tcp_conn_desc->ssthresh)
			{
				tcp_conn_desc->cwnd +=SMSS;
			}
			else 
			{
				tcp_conn_desc->cwnd += (SMSS * SMSS) / tcp_conn_desc->cwnd;
			}
			//At the moment rtt is not updated in case duplicated packets.
			//Needed to be implemented Karm algorithm (see Tcp/ip Illustrated pag 621)
			if (_nxt_snd < _ack_seq_num)
			{
				rtt = system.time - tcp_conn_desc->last_sent_time;
				tcp_conn_desc->srtt = ((float)(SRTT_FACTOR * tcp_conn_desc->srtt) + (( 1 - SRTT_FACTOR) * rtt));
				tcp_conn_desc->rto = min(RTO_UBOUND,max(RTO_LBOUND,(RTO_VARIANCE * tcp_conn_desc->srtt)));
				tcp_conn_desc->rto = tcp_conn_desc->rto / 10;
				tcp_conn_desc->last_sent_time = system.time;
			}
		}
		else if (tcp_conn_desc->snd_queue->wnd_min == _ack_seq_num)
		{
			if ((++(tcp_conn_desc->duplicated_ack)) == 3)
			{
				tcp_conn_desc->ssthresh = max(tcp_conn_desc->flight_size / 2,2 * SMSS);
				tcp_conn_desc->cwnd = tcp_conn_desc->ssthresh + 3 * SMSS;
			}
		}
		if (tcp_conn_desc->cwnd > TCP_SND_SIZE) 
		{
			tcp_conn_desc->cwnd = TCP_SND_SIZE;
		}
		tcp_queue = tcp_conn_desc->snd_queue;
		tcp_queue->wnd_size = min(tcp_conn_desc->cwnd,tcp_conn_desc->rcv_wmd_adv);
		update_snd_window(tcp_conn_desc,ack_seq_num,data_len,0);
	}
}

//long long _chk_ovrflw(unsigned int val,unsigned int ref)
//{
//	long long xxx = ((unsigned int)val >= (unsigned int)ref ? (unsigned int)val : (long long)((long long)val + (long long)4294967295));
//	return xxx;
//}

void update_snd_window(t_tcp_conn_desc* tcp_conn_desc,u32 ack_seq_num,u32 ack_data_len,u8 skip_ack)
{
	long long buf_max;
	long long wnd_max;
	long long wnd_cur;
	long long wnd_nxt;
	u32 word_to_ack;
	u32 expected_ack;
	u32 indx = 3;
	t_tcp_snd_queue* tcp_queue = NULL;
	u8 flags = 0;
	u32 ack_num;
	u32 data_to_send;
	u32 wnd_l_limit;
	u32 wnd_r_limit;
	u32 w_size;
	u32 flight_size;
	u32 flight_size_limit;
	u32 data_len;
	u32 b_free_size;

	tcp_queue = tcp_conn_desc->snd_queue;
	ack_num = tcp_conn_desc->rcv_queue->nxt_rcv;
	data_to_send = 0;
	
	if (tcp_conn_desc->duplicated_ack > 2 && ack_seq_num == 0)
	{
		return;
	}
	if (tcp_conn_desc->duplicated_ack == 0)
	{
		wnd_max = (long long)tcp_conn_desc->snd_queue->wnd_min + (long long)tcp_conn_desc->snd_queue->wnd_size;
		wnd_cur = CHK_OVRFLW(tcp_conn_desc->snd_queue->cur,tcp_conn_desc->snd_queue->wnd_min);
		wnd_nxt = CHK_OVRFLW(tcp_conn_desc->snd_queue->nxt_snd,tcp_conn_desc->snd_queue->wnd_min);
		if (skip_ack == 0)
		{

			word_to_ack = CHK_OVRFLW(ack_seq_num,tcp_conn_desc->snd_queue->wnd_min)  - tcp_queue->wnd_min;
			tcp_queue->wnd_min = tcp_queue->wnd_min + word_to_ack;
			buf_max = (long long)tcp_conn_desc->snd_queue->wnd_min + (long long)tcp_conn_desc->snd_queue->buf_size;
			b_free_size = buf_max - wnd_cur;
			if (b_free_size >= tcp_queue->pnd_data && tcp_queue->pnd_data > 0)
			{
				if (tcp_conn_desc->process_context != NULL)
				{
					_awake(tcp_conn_desc->process_context);
					tcp_queue->pnd_data = 0;
					tcp_conn_desc->process_context = NULL;
				}
			}
		}
		if (wnd_cur <= wnd_max)
		{	
			wnd_l_limit = tcp_queue->nxt_snd;
			wnd_r_limit = tcp_queue->cur;
		}
		else if (wnd_cur >= wnd_max && wnd_nxt <= wnd_max)
		{	
			wnd_l_limit = tcp_queue->nxt_snd;
			wnd_r_limit = wnd_max;
		}
		else
		{
			//no data inside window!!!!!!!
			data_to_send = 0;
			indx = tcp_queue->nxt_snd;
			goto EXIT;
		}
		//sender silly window avoidance
		//implementation lacks receiver case!!! (this is sender case)		
		w_size = wnd_r_limit - wnd_l_limit;
		expected_ack = wnd_nxt - tcp_queue->wnd_min;
		if (expected_ack == 0)
		{
			data_to_send=w_size;
			tcp_queue->nxt_snd += data_to_send;
		}
		else if (w_size >= SMSS)
		{
			data_to_send = w_size - (w_size % SMSS);
			tcp_queue->nxt_snd += data_to_send;
		}
		else if (w_size >= tcp_conn_desc->max_adv_wnd/2)
		{
			data_to_send=w_size;
			tcp_queue->nxt_snd += data_to_send;
		}
		indx = wnd_l_limit;
		tcp_conn_desc->flight_size = wnd_nxt - tcp_queue->wnd_min;
	}
	else if (tcp_conn_desc->duplicated_ack == 1 || tcp_conn_desc->duplicated_ack == 2)
	{
		//wnd_max = tcp_queue->wnd_min + tcp_queue->wnd_size;
		wnd_max = (long long)tcp_conn_desc->snd_queue->wnd_min + (long long)tcp_conn_desc->snd_queue->wnd_size;
		wnd_cur = CHK_OVRFLW(tcp_conn_desc->snd_queue->cur,tcp_conn_desc->snd_queue->wnd_min);
		wnd_nxt = CHK_OVRFLW(tcp_conn_desc->snd_queue->nxt_snd,tcp_conn_desc->snd_queue->wnd_min);
		if (wnd_max > (long long)tcp_queue->nxt_snd)                        
		{
			w_size = wnd_max - tcp_queue->nxt_snd;               
		}
		else
		{
			w_size = 0;
		}
		flight_size = tcp_queue->nxt_snd - tcp_queue->wnd_min;
		flight_size_limit = tcp_conn_desc->cwnd + 2*SMSS;

		
		if (w_size >= SMSS && (flight_size + SMSS <= flight_size_limit) && wnd_cur >= (wnd_nxt + SMSS))
		{
			indx = tcp_queue->nxt_snd;
			tcp_queue->nxt_snd += SMSS;
			data_to_send = SMSS;
			tcp_conn_desc->flight_size = wnd_nxt - wnd_cur;
		}
	}
	else if (tcp_conn_desc->duplicated_ack == 3)
	{
		data_to_send = SMSS;
		indx = ack_seq_num;
	}
	else if (tcp_conn_desc->duplicated_ack > 3)
	{
		wnd_max = (long long)tcp_conn_desc->snd_queue->wnd_min + (long long)tcp_conn_desc->snd_queue->wnd_size;
		wnd_cur = CHK_OVRFLW(tcp_conn_desc->snd_queue->cur,tcp_conn_desc->snd_queue->wnd_min);
		wnd_nxt = CHK_OVRFLW(tcp_conn_desc->snd_queue->nxt_snd,tcp_conn_desc->snd_queue->wnd_min);		

		indx = tcp_queue->nxt_snd;
		if (wnd_max > wnd_nxt)
		{
			w_size = wnd_max - wnd_nxt;
		}
		else
		{
			w_size = 0;
		}		
		if (w_size >= SMSS && wnd_cur >= (wnd_nxt + SMSS))
		{
			tcp_queue->nxt_snd += SMSS;
			data_to_send = SMSS;
			tcp_conn_desc->flight_size = wnd_nxt - wnd_cur;
		}
	}
EXIT:
	flush_data(tcp_conn_desc,data_to_send,ack_num,indx);
	//close connection with FIN flag both client and server	
	//FIN needs retrasmission management only. No retry.
	if (tcp_queue->wnd_min == tcp_queue->cur && (tcp_conn_desc->status == FIN_WAIT_1_PENDING || tcp_conn_desc->status == LAST_ACK_PENDING ))
	{
		if (tcp_conn_desc->status == FIN_WAIT_1_PENDING)
		{
			tcp_conn_desc->status = FIN_WAIT_1;
		}
		else if (tcp_conn_desc->status == LAST_ACK_PENDING)
		{
			tcp_conn_desc->status = LAST_ACK;
		}
		if (tcp_conn_desc->pgybg_timer->ref != NULL)
		{
			ll_delete_node(tcp_conn_desc->pgybg_timer->ref);
			tcp_conn_desc->pgybg_timer->ref = NULL;
		}
		_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,ack_num,FLG_FIN | FLG_ACK,tcp_conn_desc->snd_queue->nxt_snd);
		timer_set(tcp_conn_desc->rtrsn_timer,tcp_conn_desc->rto);	
	}
}

static void flush_data(t_tcp_conn_desc* tcp_conn_desc,u32 data_to_send,u32 ack_num,u32 indx)
{
	u8 flags;
	u32 data_len;
	t_tcp_snd_queue* tcp_queue = NULL;
	u32 len_1;
	u32 len_2;
	u32 offset;
	u32 seq_num;

	tcp_queue = tcp_conn_desc->snd_queue;
	seq_num = indx;
	if (data_to_send > 0)
	{
		flags = FLG_ACK;
		timer_reset(tcp_conn_desc->pgybg_timer);
		while (data_to_send >= SMSS)
		{
			offset = SLOT_WND(seq_num,TCP_SND_SIZE);
			if (offset + SMSS <= TCP_SND_SIZE)
			{
				_SEND_PACKET_TCP(tcp_conn_desc,tcp_queue->buf + offset,SMSS,ack_num,flags,seq_num);
				seq_num += SMSS;
			}
			else 
			{
				len_1 = TCP_SND_SIZE - offset;
				len_2 = SMSS - len_1;
				_SEND_PACKET_TCP(tcp_conn_desc,tcp_queue->buf + offset,len_1,ack_num,flags,seq_num);
				seq_num += len_1;
				_SEND_PACKET_TCP(tcp_conn_desc,tcp_queue->buf,len_2,ack_num,flags,seq_num);
				seq_num += len_2;
			}
			data_to_send -= SMSS;
		}
		if (data_to_send > 0)
		{
			offset = SLOT_WND(seq_num,TCP_SND_SIZE);	
			if (offset + data_to_send <= TCP_SND_SIZE)
			{
				_SEND_PACKET_TCP(tcp_conn_desc,tcp_queue->buf + offset,data_to_send,ack_num,flags,seq_num);
				seq_num += data_to_send;
			}
			else
			{
				len_1 = TCP_SND_SIZE - offset;
				len_2 = data_to_send - len_1;
				_SEND_PACKET_TCP(tcp_conn_desc,tcp_queue->buf + offset,len_1,ack_num,flags,seq_num);
				seq_num += len_1;
				_SEND_PACKET_TCP(tcp_conn_desc,tcp_queue->buf,len_2,ack_num,flags,seq_num);
				seq_num += len_2;
			}
		}
		//timer RFC6298
		timer_set(tcp_conn_desc->rtrsn_timer,tcp_conn_desc->rto);
	}
}

void rtrsn_timer_handler(void* arg)
{
	t_tcp_conn_desc* tcp_conn_desc = NULL;
	u32 ack_num = 0;
	u32 seq_num = 0;

	tcp_conn_desc = (t_tcp_conn_desc*) arg;
	//printk("timeout !!! %d \n",tcp_conn_desc->rto);
	if (tcp_conn_desc->status == ESTABILISHED || tcp_conn_desc->status == FIN_WAIT_1_PENDING)
	{
		tcp_conn_desc->rto *= 2;
		tcp_conn_desc->rto = min(tcp_conn_desc->rto , DEFAULT_RTO);
		tcp_conn_desc->cwnd = SMSS;
		tcp_conn_desc->snd_queue->nxt_snd =  tcp_conn_desc->snd_queue->wnd_min;
		tcp_conn_desc->snd_queue->wnd_size = min(tcp_conn_desc->cwnd,tcp_conn_desc->rcv_wmd_adv);
		tcp_conn_desc->duplicated_ack = 0;
		update_snd_window(tcp_conn_desc,0,1,1);
	}
	else if (tcp_conn_desc->status == SYN_SENT)
	{
		_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,0,FLG_SYN,tcp_conn_desc->snd_queue->nxt_snd);
		tcp_conn_desc->rtrsn_timer->val = tcp_conn_desc->rto;
	}
	else if (tcp_conn_desc->status == SYN_RCVD)
	{
		ack_num = tcp_conn_desc->rcv_queue->nxt_rcv = ack_num;
		_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,ack_num,(FLG_SYN | FLG_ACK),tcp_conn_desc->snd_queue->nxt_snd);
		tcp_conn_desc->rtrsn_timer->val = tcp_conn_desc->rto;
	}
	else if (tcp_conn_desc->status == FIN_WAIT_1)
	{
		tcp_conn_desc->rtrsn_timer->val = tcp_conn_desc->rto;
		_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,tcp_conn_desc->rcv_queue->nxt_rcv,FLG_FIN | FLG_ACK,tcp_conn_desc->snd_queue->nxt_snd);
		system.flush_network = 1;
	}
	else if (tcp_conn_desc->status == LAST_ACK)
	{
		tcp_conn_desc->rtrsn_timer->val = tcp_conn_desc->rto;
		ack_num = tcp_conn_desc->rcv_queue->nxt_rcv;
		seq_num = tcp_conn_desc->snd_queue->nxt_snd;
		_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,ack_num,(FLG_FIN | FLG_ACK),seq_num);
		system.flush_network = 1;
	}
}

//Non occorre sincronizzazione sto dentro stessa deferred queue
void pgybg_timer_handler(void* arg)
{
	u8 flags = 0;
	u32 ack_num = 0;
	u32 seq_num = 0;
	t_tcp_conn_desc* tcp_conn_desc = NULL;
	
	tcp_conn_desc = (t_tcp_conn_desc*) arg;
	flags = FLG_ACK;
	ack_num = tcp_conn_desc->rcv_queue->nxt_rcv;
	seq_num = tcp_conn_desc->snd_queue->nxt_snd;			
	system.flush_network = 1;
	if (tcp_conn_desc->pgybg_timer->ref != NULL && tcp_conn_desc->rcv_queue->wnd_size > 1400)
	{
		_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,ack_num,flags,seq_num);
		timer_reset(tcp_conn_desc->pgybg_timer);
		tcp_conn_desc->rcv_queue->last_adv_wnd = tcp_conn_desc->rcv_queue->wnd_size;
	}
	else
	{
		tcp_conn_desc->pgybg_timer->val = PIGGYBACKING_TIMEOUT;
	}
}

unsigned int _rand();

int send_packet_tcp(u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port,u32 wnd_size,char* data,u32 data_len,u32 ack_num,u8 flags,u32 seq_num)
{
	static counter = 0;
	u16 chk;
	char* tcp_payload = NULL;
	t_data_sckt_buf* data_sckt_buf = NULL;
	int ret = NULL;
	char* tcp_header = NULL;
	u32 tcp_header_len;

	if (flags & FLG_SYN)
	{
		tcp_header_len = HEADER_TCP + 4;
	}
	else
	{
		tcp_header_len = HEADER_TCP;
	}

	data_sckt_buf = alloc_sckt(data_len + HEADER_ETH + HEADER_IP4 + tcp_header_len);
	data_sckt_buf->transport_hdr = data_sckt_buf->data + HEADER_ETH + HEADER_IP4;
	data_sckt_buf->network_hdr=data_sckt_buf->transport_hdr-HEADER_IP4;
	tcp_payload = data_sckt_buf->transport_hdr + tcp_header_len;

	kmemcpy(tcp_payload,data,data_len);
	tcp_header = data_sckt_buf->transport_hdr;
	chk = SWAP_WORD(checksum_tcp((unsigned short*) tcp_header,src_ip,dst_ip,data_len,tcp_header_len));

	tcp_header[0] = HI_16(src_port);                            //HI SRC PORT
	tcp_header[1] = LOW_16(src_port);                           //LOW SRC PORT
	tcp_header[2] = HI_16(dst_port);                            //HI DST PORT
	tcp_header[3] = LOW_16(dst_port);                           //LOW DST PORT

	tcp_header[4] = HI_OCT_32(seq_num);                         //HIGH SEQ NUM
	tcp_header[5] = MID_LFT_OCT_32(seq_num);                    //MID LEFT SEQ NUM
	tcp_header[6] = MID_RGT_OCT_32(seq_num);                    //MID RIGHT SEQ NUM
	tcp_header[7] = LOW_OCT_32(seq_num);                        //LOW SEQ NUM

	tcp_header[8] = HI_OCT_32(ack_num);                         //HIGH ACK NUM
	tcp_header[9] = MID_LFT_OCT_32(ack_num);                    //MID LEFT ACK NUM
	tcp_header[10] = MID_RGT_OCT_32(ack_num);                   //MID RIGHT ACK NUM
	tcp_header[11] = LOW_OCT_32(ack_num);                       //LOW ACK NUM

	if ((flags & FLG_SYN) > 0)
	{
		tcp_header[12] = 0x60;                              //HEADER LEN + 4 RESERVED BIT (5 << 4)
	}
	else
	{
		tcp_header[12] = 0x50;                              //HEADER LEN + 4 RESERVED BIT (5 << 4)
	}	
	tcp_header[13] = flags;                                     //FLAGS
	tcp_header[14] = HI_16(wnd_size);                           //HI WINDOW SIZE
	tcp_header[15] = LOW_16(wnd_size);                          //LOW WINDOW SIZE

	tcp_header[16] = 0;                                         //HI TCP CHECKSUM
	tcp_header[17] = 0;                                         //LOW TCP CHECKSUM
	tcp_header[18] = 0;                                         //HI URGENT POINTER (NOT USED)
	tcp_header[19] = 0;                                         //LOW URGENT POINTER (NOT USED)
	
	if ((flags & FLG_SYN) > 0)
	{
		tcp_header[20] = 0x2;				   //MSS OPTION:TYPE
		tcp_header[21] = 0x4;                              //MSS OPTION:LENGTH
		tcp_header[22] = 0x05;                             //MSS OPTION:1454
		tcp_header[23] = 0xAE;                             //MSS OPTION:1454
	}

	chk = SWAP_WORD(checksum_tcp((unsigned short*) tcp_header,src_ip,dst_ip,data_len,tcp_header_len));
	tcp_header[16] = HI_16(chk);
	tcp_header[17] = LOW_16(chk);

	ret = send_packet_ip4(data_sckt_buf,
			    src_ip,
			    dst_ip,
			    (data_len + tcp_header_len),
			    TCP_PROTOCOL);
	return ret;
}

static u16 checksum_tcp(char* tcp_row_packet,u32 src_ip,u32 dst_ip,u16 data_len,u8 tcp_header_len)
{
	u16 packet_len;
	u16 chk;
	u16 chk_virt;
	unsigned char header_virt[16];
	unsigned char chk_final[4];
	
	header_virt[0] = HI_OCT_32(src_ip);          	
	header_virt[1] = MID_LFT_OCT_32(src_ip);
	header_virt[2] = MID_RGT_OCT_32(src_ip);
	header_virt[3] = LOW_OCT_32(src_ip);
	header_virt[4] = HI_OCT_32(dst_ip);          	
	header_virt[5] = MID_LFT_OCT_32(dst_ip);
	header_virt[6] = MID_RGT_OCT_32(dst_ip);
	header_virt[7] = LOW_OCT_32(dst_ip);		
	header_virt[8] = 0;
	header_virt[9] = TCP_PROTOCOL;
	header_virt[10] = HI_16(tcp_header_len + data_len);
	header_virt[11] = LOW_16(tcp_header_len + data_len);

	packet_len = tcp_header_len + data_len;	
	chk=checksum((unsigned short*) tcp_row_packet,packet_len);
	chk_virt=checksum((unsigned short*) header_virt,12);

	chk_final[0] = LOW_16(~chk_virt);
	chk_final[1] = HI_16(~chk_virt);
	chk_final[2] = LOW_16(~chk);
	chk_final[3] = HI_16(~chk);
	return checksum(chk_final,4);
}
#define RAND_MAX 4294967294

unsigned int _rand()
{
	static u32 seed;
        seed = seed * 1103515245 + 12345;
        return (seed % ((unsigned int)RAND_MAX + 1));
}
