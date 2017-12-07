#include "network/network.h"

//extern int go;
//extern int ggo;
//int pp1 = 0;
//int pp2 = 0;

//ONLY ONE INSTANCE FOR ALL POSSIBLE INTERFACES!!!!!
t_network_desc* network_init()
{
	t_network_desc* network_desc;

	network_desc=kmalloc(sizeof(t_network_desc));
	network_desc->tx_queue=sckt_buf_desc_init();
	network_desc->rx_queue=sckt_buf_desc_init();
	network_desc->dev=init_8254x();
	network_desc->ip=LOCAL_IP;
	network_desc->netmask=LOCAL_NETMASK;
	network_desc->default_gw_ip=DEFAULT_GW_IP;
	network_desc->socket_desc=socket_desc_init();
	arp_init();
	network_desc->tcp_desc=tcp_init();
	return network_desc;
}

void network_free(t_network_desc* network_desc)
{
	sckt_buf_desc_free(network_desc->tx_queue);
	sckt_buf_desc_free(network_desc->rx_queue);
	free_8254x(network_desc->dev);
	socket_desc_free(network_desc->socket_desc);
	tcp_free(network_desc->tcp_desc);
	kfree(network_desc);
}

void equeue_packet(t_network_desc* network_desc)
{
	int stop;
	t_sckt_buf_desc* sckt_buf_desc;
	t_data_sckt_buf* data_sckt_buf;
	void* frame;
	u16 frame_len;
	
//	u32 tot_sent = 0;
//	u32 static count = 0;
//	u32 static tot_count = 0;
//	u32 static avg = 0;
//	static int tot = 0;
//	static int t1 = 0;
//	static int t2 = 0;
//	static int iter = 0;
//	
//	iter++;
//	if (iter > 1)
//	{
//		printk("race !!! \n"); 
//	}
	
//	DISABLE_PREEMPTION
//	STI
	sckt_buf_desc=network_desc->tx_queue;
	stop = 0;
	while (stop == 0 && (data_sckt_buf=dequeue_sckt(sckt_buf_desc))!=NULL)
	{
		frame=data_sckt_buf->mac_hdr;
		frame_len=data_sckt_buf->data_len;
//		CLI
		send_packet_i8254x(network_desc->dev,frame,frame_len);
//		STI
		free_sckt(data_sckt_buf);
//		tot_sent += frame_len;
//		if (system.force_scheduling == 1 || system.preempt_network_flush == 1)
//		{
//			system.preempt_network_flush = 0;
//			stop = 1;
//		}
	}
//	CLI
//	ENABLE_PREEMPTION
	
//	iter--;
//	if (iter > 0)
//	{
//		printk("race !!! \n"); 
//	}
//	if (ggo == 1)
//	{
//		tot += tot_sent;
//		tot_count++;
//		if (tot_sent > 0)
//		{
//			count++;
//			avg = tot / count;
//		}
//		if (tot_sent <= 16384)
//		{
//			t1++;
//		}
//		else 
//		{
//			t2++;
//		}
//		if (tot > 31200000)preempt_network_flush
//		{
//			printk("break!!! \n");
//		}
//	}
}

void dequeue_packet(t_network_desc* network_desc)
{
	int i=0;
	t_data_sckt_buf* data_sckt_buf;
	
//	static int iter = 0;
//
//	iter++;
//	if (iter > 1)
//	{
//		printk("race !!! \n"); 
//	}

//	DISABLE_PREEMPTION
//	STI
	while ((data_sckt_buf=dequeue_sckt(network_desc->rx_queue))!=NULL)
	{	
//		CLI
		rcv_packet_mac(data_sckt_buf);
		i++;
//		STI
		if (system.force_scheduling != 0)
		{
			break;
		}
	}
//	CLI
//	ENABLE_PREEMPTION

//	iter--;
//	if (iter > 0)
//	{
//		printk("race !!! \n"); 
//	}
}

void debug_network(char* data,u32 data_len)
{
	char* ip_payload;
        t_data_sckt_buf* data_sckt_buf=alloc_sckt(33+HEADER_ETH+HEADER_IP4+HEADER_UDP);
	data_sckt_buf->transport_hdr=data_sckt_buf->data+HEADER_ETH+HEADER_IP4;
	
	ip_payload=data_sckt_buf->transport_hdr+HEADER_UDP;
	u32 src_ip=GET_DWORD(172,16,243,101);
	u32 dst_ip=GET_DWORD(172,16,243,1);

	kmemcpy(ip_payload,data,data_len);
	send_packet_udp(data_sckt_buf,src_ip,dst_ip,9999,45446,data_len);
}
