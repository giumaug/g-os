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
	t_tcp_conn_desc* tmp;
	int ret;
	
	ret = -1;
	tmp = tcp_conn_map_get(tcp_desc->listen_map,tcp_conn_desc->src_ip,0,tcp_conn_desc->src_port,0);
	if (tcp_listen_desc == NULL)
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

void connect_tpc(t_tcp_conn_map* tcp_req_map,src_ip,dst_ip,src_port,dst_port)
{
	u32 src_port;
	t_tcp_conn_desc* tcp_conn_desc = NULL;

	src_port = free_port_search();
	if (src_port == NULL)
	{
		return -1;
	}
	tcp_conn_desc = tcp_conn_desc_int();
	tcp_conn_desc->dst_ip = dst_ip;
	tcp_conn_desc->dst_port = dst_port;
	tcp_conn_desc->src_ip = src_ip;
	tcp_conn_desc->src_port = src_port;
	tcp_conn_desc->status = SYN_SENT;

	tcp_conn_map_put(tcp_req_map,src_ip,dst_ip,src_port,dst_port,tcp_conn_desc);
	send_packet_tcp(tcp_conn_desc,NULL,0,0,FLG_SYN);
	return 0;
}

void close_tcp(t_tcp_conn_desc tcp_conn_desc*)
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
	DISABLE_PREEMPTION
	available_data = (tcp_queue->nxt_rcv - 1) > tcp_queue->wnd_min;
	while (available_data == 0)
	{
		enqueue(tcp_conn_desc->rcv_queue,current_process_context);
		CLI
		ENABLE_PREEMPTION
		_sleep();
		//I use CLI to avoid a new context switch
		CLI
		DISABLE_PREEMPTION
		STI
		available_data = (tcp_queue->nxt_rcv - 1) > tcp_queue->wnd_min;
	}

	if (available_data > 0 && available_data < data_len)
	{
		data_len = available_data;
	}

	low_index = tcp_queue->buf_min;
	hi_index = SLOT_WND(tcp_queue->buf_min + data_len,tcp_queue->buf_size);

	if (low_index < hi_index) 
	{
		kmemcpy(data,tcp_queue->buf_min,data_len);
		for (i = low_index;i <= hi_index;i++)
		{
			bit_vector_reset(tcp_queue->buf_state,state_index);
		}
	}
	else 
	{
		len_1 = tcp_queue->size - low_index;
		len_2 = data_len - len_1;
		kmemcpy(data,tcp_queue->buf_min,len_1);
		kmemcpy(data + len_1,tcp_queue->buf_min,len_2);

		for (i = low_index;i <= len_1;i++)
		{
			bit_vector_reset(tcp_queue->buf_state,state_index);
		}
		for (i = 0;i <= len_2;i++)
		{
			bit_vector_reset(tcp_queue->buf_state,state_index);
		}
	}
	tcp_queue->wnd_size += data_len;
EXIT:
	//ENABLE PREEMPTION OR SOFT IRQ
	ENALBLE_PREEMPTION
	return ret;
}

//Meglio scodare solo dentro la deferred queue,serve pure un meccanismo che
//avvia lo scodamentom solo se non e' stato lanciato in precedenza dal
//processamento dell'ack
int enqueue_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len)
{
	t_tcp_snd_queue* tcp_queue = tcp_conn_desc->snd_queue;

	//DISABLE PREEMPTION OR SOFT IRQ
	DISABLE_PREEMPTION
	b_free_size = WND_SIZE(tcp_queue->tcp_snd_queue->buf_cur,tcp_queue->buf_max);
	if (b_free_size < data_len)
	{
		return -1;
	}
	if (tcp_snd_queue->cur < tcp_queue->buf_max) 
	{
		kmemcpy(tcp_queue->buf[tcp_snd_queue->buf_cur],data,data_len);
		INC_WND(tcp_snd_queue->buf_cur,tcp_snd_queue->buf_size,data_len);
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
	//ENABLE PREEMPTION OR SOFT IRQ
	ENABLE_PREEMPTION
	
	//vedi commento sopra
	//update_snd_window(tcp_conn_desc,0,1);
}

