#include "network/tcp_socket.h"

int ggo = 0;
static int free_port_search()
{
	int i;
	void* port = NULL;
	t_tcp_desc* tcp_desc = NULL;
	t_tcp_conn_desc* tmp = NULL;

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

int bind_tcp(t_tcp_conn_desc* tcp_conn_desc,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port)
{
	SAVE_IF_STATUS
	CLI
	tcp_conn_desc->src_ip = src_ip;
	tcp_conn_desc->src_port = src_port;
	//All incoming request are wildcarded
	tcp_conn_desc->dst_ip = 0;
	tcp_conn_desc->dst_port = 0;
	RESTORE_IF_STATUS
	return 0;
}

int listen_tcp(t_tcp_conn_desc* tcp_conn_desc)
{
	int ret;
	t_tcp_desc* tcp_desc = NULL;
	t_tcp_conn_desc* tmp = NULL;

	SAVE_IF_STATUS
	CLI
	tcp_desc = system.network_desc->tcp_desc;
	ret = -1;
	tmp = tcp_conn_map_get(tcp_desc->listen_map,tcp_conn_desc->src_ip,0,tcp_conn_desc->src_port,0);
	if (tmp == NULL)
	{
		tcp_conn_map_put(tcp_desc->listen_map,tcp_conn_desc->src_ip,0,tcp_conn_desc->src_port,0,tcp_conn_desc);
		ret = 0;
	}
	RESTORE_IF_STATUS
	return ret;
}

t_tcp_conn_desc* accept_tcp(t_tcp_conn_desc* tcp_conn_desc)
{
	t_tcp_conn_desc* new_tcp_conn_desc = NULL;

	SAVE_IF_STATUS
	CLI 
	new_tcp_conn_desc = dequeue(tcp_conn_desc->back_log_c_queue);

	if (new_tcp_conn_desc == NULL)
	{
		CURRENT_PROCESS_CONTEXT(tcp_conn_desc->process_context);
		_sleep();
		new_tcp_conn_desc = dequeue(tcp_conn_desc->back_log_c_queue);
	}
	new_tcp_conn_desc->status = ESTABILISHED;
	RESTORE_IF_STATUS
	return new_tcp_conn_desc;
}

int connect_tcp(u32 dst_ip,u16 dst_port,t_socket* socket)
{
	u16 src_port;
	u32 src_ip;
	t_tcp_conn_desc* tcp_conn_desc = NULL;
	t_tcp_conn_map* tcp_req_map = NULL;

	SAVE_IF_STATUS
	CLI
	tcp_req_map = system.network_desc->tcp_desc->req_map;
	src_port = free_port_search();
	if (src_port == NULL)
	{
		RESTORE_IF_STATUS
		return -1;
	}
	src_ip = system.network_desc->ip;
	tcp_conn_desc = tcp_conn_desc_int();
	tcp_conn_desc->ref_count = 1;
	tcp_conn_desc->dst_ip = dst_ip;
	tcp_conn_desc->dst_port = dst_port;
	tcp_conn_desc->src_ip = src_ip;
	tcp_conn_desc->src_port = src_port;
	tcp_conn_desc->status = SYN_SENT;
	tcp_conn_desc->rtrsn_timer->val = tcp_conn_desc->rto;
	rtrsn_timer_set(tcp_conn_desc->rtrsn_timer,tcp_conn_desc->rto);
	socket->tcp_conn_desc = tcp_conn_desc;
	tcp_conn_map_put(tcp_req_map,src_ip,dst_ip,src_port,dst_port,tcp_conn_desc);
	//SYN NEED RETRASMISSION TIMEOUT MANAGEMENT ONLY.NO RETRY	
	_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,0,FLG_SYN,tcp_conn_desc->snd_queue->nxt_snd);
	CURRENT_PROCESS_CONTEXT(tcp_conn_desc->process_context);
	_sleep();
	RESTORE_IF_STATUS
	return 0;
}

void close_tcp(t_tcp_conn_desc* tcp_conn_desc)
{
	t_tcp_desc* tcp_desc = NULL;
	u8 flags = 0;
	u32 ack_num = 0;
	u32 seq_num = 0;	

	SAVE_IF_STATUS
	CLI
	tcp_desc = system.network_desc->tcp_desc;
	flags = FLG_FIN | FLG_ACK;
	seq_num = seq_num = tcp_conn_desc->snd_queue->nxt_snd;
	ack_num = tcp_conn_desc->last_ack_sent;

	if (tcp_conn_desc->status == CLOSED)
	{
		tcp_conn_map_remove(tcp_desc->listen_map,tcp_conn_desc->src_ip,tcp_conn_desc->dst_ip,tcp_conn_desc->src_port,tcp_conn_desc->dst_port);
	}
	if (tcp_conn_desc->snd_queue->wnd_min == tcp_conn_desc->snd_queue->cur)
	{
		if (tcp_conn_desc->pgybg_timer->ref != NULL)
		{
			ack_num = tcp_conn_desc->rcv_queue->nxt_rcv;
			ll_delete_node(tcp_conn_desc->pgybg_timer->ref);
			tcp_conn_desc->pgybg_timer->ref = NULL;
		}
		flags |= FLG_ACK;
		_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,ack_num,flags,seq_num);
		if (tcp_conn_desc->status == ESTABILISHED)
		{
			//FIN from client to server
			tcp_conn_desc->status = FIN_WAIT_1;
		}	
		else if (tcp_conn_desc->status == CLOSE_WAIT) 
		{
			//FIN from server to client
			tcp_conn_desc->status = LAST_ACK;
		}
	}
	if (tcp_conn_desc->status == ESTABILISHED)
	{
		//FIN from client to server
		tcp_conn_desc->status = FIN_WAIT_1_PENDING;
	}
	else if (tcp_conn_desc->status == CLOSE_WAIT) {

		//FIN from server to client
		tcp_conn_desc->status = LAST_ACK_PENDING;
	}
	RESTORE_IF_STATUS
	return;
}

int dequeue_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len)
{
	u32 i = 0;
	u32 len_1 = 0;
	u32 len_2 = 0;
	u32 index = 0;
	u32 low_index = 0;
	u32 hi_index = 0;
	u32 available_data = 0;
	struct t_process_context* current_process_context;
	t_tcp_rcv_queue* tcp_queue = NULL;
	
	SAVE_IF_STATUS
	CLI
	CURRENT_PROCESS_CONTEXT(current_process_context);
	tcp_queue = tcp_conn_desc->rcv_queue;
	available_data = tcp_queue->nxt_rcv - tcp_queue->wnd_min;
	while (available_data == 0)
	{
		enqueue(tcp_conn_desc->data_wait_queue,current_process_context);
		_sleep();
		available_data = tcp_queue->nxt_rcv - tcp_queue->wnd_min;
		if (available_data == 0)
		{
			return 0;
		}
	}

	if (available_data > 0 && available_data < data_len)
	{
		data_len = available_data;
	}

	low_index = SLOT_WND(tcp_queue->wnd_min,tcp_queue->buf_size);
	hi_index = SLOT_WND((tcp_queue->wnd_min + data_len),tcp_queue->buf_size);

	if (low_index < hi_index) 
	{
		kmemcpy(data,(tcp_queue->buf + low_index),data_len);
		for (i = low_index;i < hi_index;i++)
		{
			bit_vector_reset(tcp_queue->buf_state,i);
		}
	}
	else 
	{
		len_1 = tcp_queue->buf_size - low_index;
		len_2 = data_len - len_1;
		kmemcpy(data,(tcp_queue->buf + low_index),len_1);
		kmemcpy(data + len_1,tcp_queue->buf,len_2);

		for (i = low_index ; i < (low_index + len_1) ; i++)
		{
			bit_vector_reset(tcp_queue->buf_state,i);
		}
		for (i = 0;i < len_2;i++)
		{
			bit_vector_reset(tcp_queue->buf_state,i);
		}
	}
	tcp_queue->wnd_size += data_len;
	tcp_queue->wnd_min += data_len;

	RESTORE_IF_STATUS
	return data_len;
}

//Meglio scodare solo dentro la deferred queue,serve pure un meccanismo che
//avvia lo scodamento solo se non e' stato lanciato in precedenza dal
//processamento dell'ack
//CURRENTLY IT WORKS WITH INTERRUPTS DISABLED
int enqueue_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len)
{
	t_tcp_snd_queue* tcp_queue = NULL;
	u32 cur_index;
	u32 b_free_size;
	u32 wnd_max;
	u32 len_1;
	u32 len_2;

	SAVE_IF_STATUS
	CLI
	if (tcp_conn_desc->status == ESTABILISHED || tcp_conn_desc->status == CLOSE_WAIT)
	{
		tcp_queue = tcp_conn_desc->snd_queue;
		wnd_max = tcp_queue->wnd_min + tcp_queue->buf_size;
		b_free_size = wnd_max - tcp_queue->cur;
		//printk("free size is:%d \n",b_free_size);
		if (b_free_size < data_len)
		{
			RESTORE_IF_STATUS
			return -1;
		}
		else
		{
			cur_index = SLOT_WND(tcp_queue->cur,tcp_queue->buf_size);
			if ((tcp_queue->buf_size - cur_index) > data_len)
			{
				kmemcpy(tcp_queue->buf + cur_index,data,data_len);
				tcp_queue->cur += data_len;
			}
			else
			{
				len_1 = tcp_queue->buf_size - cur_index;
				len_2 = data_len - len_1;
				kmemcpy(tcp_queue->buf + cur_index,data,len_1);
				kmemcpy(tcp_queue->buf,data,len_2);
				tcp_queue->cur += data_len;
			}
		}
		//vedi commento sopra
		update_snd_window(tcp_conn_desc,0,1);
	}
	RESTORE_IF_STATUS
	return 0;
}
