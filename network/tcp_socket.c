#include "lib/lib.h"
#include "network/tcp_socket.h"

static int free_port_search()
{
	int i;
	void* port = NULL;
	t_tcp_desc* tcp_desc = NULL;

	tcp_desc = system.network_desc->tcp_desc;

	for (i=0;i<32767;i++)
	{
		tcp_desc->ep_port_index++;
		if (tcp_desc->ep_port_index > 65535) 
		{
			tcp_desc->ep_port_index = 32767;
		}
		port = hashtable_get(tcp_desc->ep_port_map,tcp_desc->ep_port_index);
		if (port == NULL)
		{
			hashtable_put(tcp_desc->ep_port_map,tcp_desc->ep_port_index,1);
			return tcp_desc->ep_port_index;
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
		printk("no port found \n");
		RESTORE_IF_STATUS
		return -1;
	}
	src_ip = system.network_desc->ip;
	tcp_conn_desc = tcp_conn_desc_int(EPHEMERAL_PORT_MAP_SIZE);
	tcp_conn_desc->isActive = 1;
	tcp_conn_desc->ref_count = 1;
	tcp_conn_desc->dst_ip = dst_ip;
	tcp_conn_desc->dst_port = dst_port;
	tcp_conn_desc->src_ip = src_ip;
	tcp_conn_desc->src_port = src_port;
	tcp_conn_desc->status = SYN_SENT;
	timer_set(tcp_conn_desc->rtrsn_timer,tcp_conn_desc->rto);
	socket->tcp_conn_desc = tcp_conn_desc;
	tcp_conn_map_put(tcp_req_map,src_ip,dst_ip,src_port,dst_port,tcp_conn_desc);
	_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,0,FLG_SYN,tcp_conn_desc->snd_queue->nxt_snd);
	CURRENT_PROCESS_CONTEXT(tcp_conn_desc->process_context);
	system.flush_network = 1;
	_sleep();
	RESTORE_IF_STATUS
	return 0;
}

void close_tcp(t_tcp_conn_desc* tcp_conn_desc)
{
	struct t_process_context* process_context = NULL;
	t_tcp_desc* tcp_desc = NULL;
	t_tcp_conn_desc* tmp_tcp_conn_desc = NULL;
	t_tcp_conn_map*  tmp_tcp_conn_map = NULL;
	u8 flags = 0;
	u32 ack_num = 0;
	u32 seq_num = 0;	

	SAVE_IF_STATUS
	CLI
	tcp_desc = system.network_desc->tcp_desc;
	flags = FLG_FIN | FLG_ACK;
	seq_num = seq_num = tcp_conn_desc->snd_queue->nxt_snd;
	ack_num = tcp_conn_desc->rcv_queue->nxt_rcv;	
	CURRENT_PROCESS_CONTEXT(process_context);

	if (tcp_conn_desc->status == RESET || process_context->sig_num == SIGINT)
	{
		tmp_tcp_conn_desc = tcp_conn_map_get(tcp_desc->conn_map,
						tcp_conn_desc->src_ip,
						tcp_conn_desc->dst_ip,
						tcp_conn_desc->src_port,
						tcp_conn_desc->dst_port);
		if (tmp_tcp_conn_desc != NULL)
		{
			tmp_tcp_conn_map = tcp_desc->conn_map;
		}
		else
		{
			tmp_tcp_conn_desc = tcp_conn_map_get(tcp_desc->listen_map,
					 	             tcp_conn_desc->src_ip,
						             tcp_conn_desc->dst_ip,
						             tcp_conn_desc->src_port,
						             tcp_conn_desc->dst_port);

			if (tmp_tcp_conn_desc != NULL)
			{
				tmp_tcp_conn_map = tcp_desc->listen_map;
			}
			else 
			{
				tmp_tcp_conn_desc = tcp_conn_map_get(tcp_desc->req_map,
					 	             tcp_conn_desc->src_ip,
						             tcp_conn_desc->dst_ip,
						             tcp_conn_desc->src_port,
						             tcp_conn_desc->dst_port);
			
				if (tmp_tcp_conn_desc != NULL)
				{
					tmp_tcp_conn_map = tcp_desc->req_map;
				}
			}	
		}
                if (tmp_tcp_conn_map != NULL)
		{
			tcp_conn_map_remove(tmp_tcp_conn_map,
				            tcp_conn_desc->src_ip,
				            tcp_conn_desc->dst_ip,
				            tcp_conn_desc->src_port,
				            tcp_conn_desc->dst_port);
			tcp_conn_desc_free(tmp_tcp_conn_desc);
		}
	}
	else
	{ 
		if (tcp_conn_desc->status == CLOSED)
		{
			tcp_conn_map_remove(tcp_desc->listen_map,
				    	    tcp_conn_desc->src_ip,
				            tcp_conn_desc->dst_ip,
				            tcp_conn_desc->src_port,
				            tcp_conn_desc->dst_port);
		}
		if (tcp_conn_desc->snd_queue->wnd_min == tcp_conn_desc->snd_queue->cur)
		{
			flags |= FLG_ACK;
			_SEND_PACKET_TCP(tcp_conn_desc,NULL,0,ack_num,flags,seq_num);
			timer_set(tcp_conn_desc->rtrsn_timer,tcp_conn_desc->rto);
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
	}
	RESTORE_IF_STATUS
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
	t_tcp_rcv_queue* tcp_queue = NULL;
	
	int log_index;
	int log_index_old;
	
//	DISABLE_PREEMPTION
	SAVE_IF_STATUS
//	SAVE_IF_STATUS
	CLI
	system.flush_network = 0;

//	RESTORE_IF_STATUS
	#ifdef PROFILE
	long long time_1;
	long long time_2;
	rdtscl(&time_1);
	#endif
	CURRENT_PROCESS_CONTEXT(current_process_context);
	if (tcp_conn_desc->status == RESET)
	{
		panic();
		printk("reset on read!!!! \n");
		ret = -1;
	}
	else if (tcp_conn_desc->status == ESTABILISHED || tcp_conn_desc->status == CLOSE_WAIT)
	{
//		tcp_socket_dump_1(tcp_conn_desc);	
		tcp_queue = tcp_conn_desc->rcv_queue;
		available_data = tcp_queue->nxt_rcv - tcp_queue->wnd_min;
		if (tcp_conn_desc->status == CLOSE_WAIT && available_data == 0)
		{
			ret = 0;
			goto EXIT;
		}
		while (available_data == 0)
		{
			//tcp_conn_desc->log.item[tcp_conn_desc->log.index].sleep = 1;
			tcp_conn_desc->process_context = current_process_context;
			system.flush_network = 1;
			_sleep();
			available_data = tcp_queue->nxt_rcv - tcp_queue->wnd_min;
			if (available_data == 0)
			{
				ret = -1;
				//printk("error ....\n");
				goto EXIT;
			}
		}
		if (available_data > 0 && available_data < data_len)
		{
			data_len = available_data;
		}
		ret = data_len;
		low_index = SLOT_WND(tcp_queue->wnd_min,tcp_queue->buf_size);
		hi_index = SLOT_WND((tcp_queue->wnd_min + data_len),tcp_queue->buf_size);
		if (low_index < hi_index)
		{
			kmemcpy(data,(tcp_queue->buf + low_index),data_len);	
		}
		else 
		{
			len_1 = tcp_queue->buf_size - low_index;
			len_2 = data_len - len_1;
			kmemcpy(data,(tcp_queue->buf + low_index),len_1);
			kmemcpy(data + len_1,tcp_queue->buf,len_2);	
		}
		
		tcp_queue->wnd_size += data_len;
		tcp_queue->wnd_min += data_len;
		
//		tcp_socket_dump_2(tcp_conn_desc);
	
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
EXIT:
//	_SAVE_IF_STATUS
//	CLI
	#ifdef PROFILE
	rdtscl(&time_2);
	system.tot_rcv_time += (time_2 - time_1);
	#endif
	system.flush_network = 1;
	RESTORE_IF_STATUS
//	ENABLE_PREEMPTION
	return ret;
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
	long long wnd_max;
	u32 len_1;
	u32 len_2;
	int ret = 0;	

	SAVE_IF_STATUS
	CLI
	if (tcp_conn_desc->status == RESET)
	{
		panic();
		printk("reset on write!!!! \n");
		ret = -1;
	}
	else if (tcp_conn_desc->status == ESTABILISHED || tcp_conn_desc->status == CLOSE_WAIT)
	{
		tcp_queue = tcp_conn_desc->snd_queue;
		wnd_max = (long long)tcp_queue->wnd_min + (long long)tcp_queue->buf_size;
		b_free_size = wnd_max - CHK_OVRFLW(tcp_queue->cur,tcp_queue->wnd_min);
		if (b_free_size < data_len)
		{
			CURRENT_PROCESS_CONTEXT(tcp_conn_desc->process_context);
			tcp_queue->pnd_data = data_len;
			_sleep();
			wnd_max = tcp_queue->wnd_min + tcp_queue->buf_size;
			b_free_size = wnd_max - CHK_OVRFLW(tcp_queue->cur,tcp_queue->wnd_min);
			if (b_free_size < data_len)
			{
				ret = -1;
				printk("error +++\n");
				goto EXIT;
			}
		}
		ret = data_len;
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
		//vedi commento sopra
		update_snd_window(tcp_conn_desc,0,1,1);
		system.flush_network = 1;
	}
EXIT:
	RESTORE_IF_STATUS
	return ret;
}
