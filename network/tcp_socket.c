#include "network/tcp_socket.h"

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
	tcp_conn_desc->src_ip = src_ip;
	tcp_conn_desc->src_port = src_port;
	//All incoming request are wildcarded
	tcp_conn_desc->dst_ip = 0;
	tcp_conn_desc->dst_port = 0;
	return 0;
}

int listen_tcp(t_tcp_conn_desc* tcp_conn_desc)
{
	int ret;
	t_tcp_desc* tcp_desc = NULL;
	t_tcp_conn_desc* tmp = NULL;

	tcp_desc = system.network_desc->tcp_desc;
	ret = -1;
	tmp = tcp_conn_map_get(tcp_desc->listen_map,tcp_conn_desc->src_ip,0,tcp_conn_desc->src_port,0);
	if (tmp == NULL)
	{
		tcp_conn_map_put(tcp_desc->listen_map,tcp_conn_desc->src_ip,0,tcp_conn_desc->src_port,0,tcp_conn_desc);
		ret = 0;
	}
	return ret;
}

t_tcp_conn_desc* accept_tcp(t_tcp_conn_desc* tcp_conn_desc)
{
	t_tcp_conn_desc* new_tcp_conn_desc = NULL;

	new_tcp_conn_desc = dequeue(tcp_conn_desc->back_log_c_queue);
	new_tcp_conn_desc->status = ESTABILISHED;
	if (new_tcp_conn_desc != NULL)
	{
		return new_tcp_conn_desc;
	}
	return NULL;
}

int connect_tcp(u32 dst_ip,u16 dst_port,t_socket* socket)
{
	u16 src_port;
	u32 src_ip;
	t_tcp_conn_desc* tcp_conn_desc = NULL;
	t_tcp_conn_map* tcp_req_map = NULL;

	tcp_req_map = system.network_desc->tcp_desc->req_map;
	src_port = free_port_search();
	if (src_port == NULL)
	{
		return -1;
	}
	src_ip = system.network_desc->ip;
	tcp_conn_desc = tcp_conn_desc_int();
	tcp_conn_desc->dst_ip = dst_ip;
	tcp_conn_desc->dst_port = dst_port;
	tcp_conn_desc->src_ip = src_ip;
	tcp_conn_desc->src_port = src_port;
	tcp_conn_desc->status = SYN_SENT;
	tcp_conn_desc->rtrsn_timer->val = tcp_conn_desc->rto;
	tcp_conn_desc->rtrsn_timer->ref = ll_append(system.timer_list,tcp_conn_desc->rtrsn_timer);
	socket->tcp_conn_desc = tcp_conn_desc;
	tcp_conn_map_put(tcp_req_map,src_ip,dst_ip,src_port,dst_port,tcp_conn_desc);
	//SYN NEED RETRASMISSION TIMEOUT MANAGEMENT ONLY.NO RETRY	
	send_packet_tcp(tcp_conn_desc,NULL,0,0,FLG_SYN);

	return 0;
}

void close_tcp(t_tcp_conn_desc* tcp_conn_desc)
{
	if (tcp_conn_desc->status = ESTABILISHED)
	{
		//FIN from client to server
		tcp_conn_desc->status = FIN_WAIT_1;
	}
	else if (tcp_conn_desc->status = CLOSE_WAIT) {

		//FIN from server to client
		tcp_conn_desc->status = LAST_ACK;
	}
	return;
}

//CURRENTLY IT WORKS WITH INTERRUPTS DISABLED
int dequeue_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len)
{
	int ret = 0;
	u32 i = 0;
	u32 len_1 = 0;
	u32 len_2 = 0;
	u32 index = 0;
	u32 low_index = 0;
	u32 hi_index = 0;
	u32 available_data = 0;
	struct t_process_context* current_process_context;
	t_tcp_rcv_queue* tcp_queue = tcp_conn_desc->rcv_queue;
	
	CURRENT_PROCESS_CONTEXT(current_process_context);
	//DISABLE PREEMPTION OR SOFT IRQ
//	DISABLE_PREEMPTION
	available_data = (tcp_queue->nxt_rcv - 1) > tcp_queue->wnd_min;
	while (available_data == 0)
	{
		enqueue(tcp_conn_desc->data_wait_queue,current_process_context);
//		CLI
//		ENABLE_PREEMPTION
		_sleep();
		//I use CLI to avoid a new context switch
//		CLI
//		DISABLE_PREEMPTION
//		STI
		available_data = (tcp_queue->nxt_rcv - 1) > tcp_queue->wnd_min;
	}

	if (available_data > 0 && available_data < data_len)
	{
		data_len = available_data;
	}

	low_index = SLOT_WND(tcp_queue->wnd_min,tcp_queue->buf_size);
	hi_index = SLOT_WND(tcp_queue->wnd_min + data_len,tcp_queue->buf_size);

	if (low_index < hi_index) 
	{
		kmemcpy(data,(tcp_queue->buf + low_index),data_len);
		for (i = low_index;i <= hi_index;i++)
		{
			bit_vector_reset(tcp_queue->buf_state,i);
		}
	}
	else 
	{
		len_1 = tcp_queue->wnd_size - low_index;
		len_2 = data_len - len_1;
		kmemcpy(data,(tcp_queue->buf + hi_index),len_1);
		kmemcpy(data + len_1,(tcp_queue->buf + low_index),len_2);

		for (i = low_index;i <= len_1;i++)
		{
			bit_vector_reset(tcp_queue->buf_state,i);
		}
		for (i = 0;i <= len_2;i++)
		{
			bit_vector_reset(tcp_queue->buf_state,i);
		}
	}
	tcp_queue->wnd_size += data_len;
EXIT:
	//ENABLE PREEMPTION OR SOFT IRQ
//	ENALBLE_PREEMPTION
	return ret;
}

//Meglio scodare solo dentro la deferred queue,serve pure un meccanismo che
//avvia lo scodamento solo se non e' stato lanciato in precedenza dal
//processamento dell'ack
//CURRENTLY IT WORKS WITH INTERRUPTS DISABLED
int enqueue_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len)
{
	t_tcp_snd_queue* tcp_queue = tcp_conn_desc->snd_queue;
	u32 cur_index;
	u32 b_free_size;
	u32 offset;
	u32 wnd_max;

	//DISABLE PREEMPTION OR SOFT IRQ
//	DISABLE_PREEMPTION
	wnd_max = tcp_queue->wnd_min + tcp_queue->wnd_size;
	b_free_size = wnd_max - tcp_queue->cur;
	if (b_free_size < data_len)
	{
		return -1;
	}
// DEAD CODE?????
//	if (tcp_snd_queue->cur < tcp_queue->buf_max) 
//	{
//		kmemcpy(tcp_queue->buf[tcp_snd_queue->buf_cur],data,data_len);
//		INC_WND(tcp_snd_queue->buf_cur,tcp_snd_queue->buf_size,data_len);
//	}
	else
	{
		cur_index = SLOT_WND(tcp_queue->cur,tcp_queue->buf_size);
		if ((tcp_queue->buf_size - tcp_queue->cur) > data_len)
		{
			kmemcpy(tcp_queue->buf[tcp_queue->cur],data,data_len);
			INC_WND(tcp_queue->cur,tcp_queue->buf_size,data_len);
		}
		else
		{
			offset = data_len - (tcp_queue->buf_size - cur_index);----------qui!!!
			kmemcpy(tcp_queue->buf[tcp_queue->cur],data,data_len);
			kmemcpy(tcp_queue->buf[0],data,offset);
			INC_WND(tcp_queue->cur,tcp_queue->buf_size,data_len);
		}
	}
	//ENABLE PREEMPTION OR SOFT IRQ
//	ENABLE_PREEMPTION
	
	//vedi commento sopra
	update_snd_window(tcp_conn_desc,0,1);
}

