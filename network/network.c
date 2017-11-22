#include "network/network.h"

extern int go;

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
	t_sckt_buf_desc* sckt_buf_desc;
	t_data_sckt_buf* data_sckt_buf;
	void* frame;
	u16 frame_len;
	u32 tot_sent = 0;
	static int tot = 0;
	static int t1 = 0;
	static int t2 = 0;
	int count = 0;
	static int iter = 0;
	iter++;
//	if (iter > 1)
//	{
//		//printk("race !!! \n"); 
//	}
	
	DISABLE_PREEMPTION
////	SAVE_IF_STATUS
	STI
	sckt_buf_desc=network_desc->tx_queue;
	//while ((data_sckt_buf=dequeue_sckt(sckt_buf_desc))!=NULL && system.force_scheduling == 0)
	while ((data_sckt_buf=dequeue_sckt(sckt_buf_desc))!=NULL)
	{
		count++;
		frame=data_sckt_buf->mac_hdr;
		frame_len=data_sckt_buf->data_len;
		CLI
		send_packet_i8254x(network_desc->dev,frame,frame_len);
		STI
		free_sckt(data_sckt_buf);
		tot_sent += frame_len;
//		if (system.force_scheduling != 0)
//		{
//			break;
//		}
	}
////	RESTORE_IF_STATUS
	CLI
	ENABLE_PREEMPTION
//	iter--;---last
//	if (iter > 0)
//	{
//		//printk("race !!! \n"); 
//	}
	if (go == 1)
	{
		tot += tot_sent;
		if (tot_sent <= 16384)
		{
			t1++;
		}
		else 
		{
			t2++;
		}
		if (tot_sent != 0)
		{
			//printk("sent: %d \n",tot_sent);
			//printk("count: %d \n",count);
		}
		if (tot > 31200000)
		{
			//printk("break!!! \n");
		}
	}
}

void dequeue_packet(t_network_desc* network_desc)
{
	int i=0;
	t_data_sckt_buf* data_sckt_buf;
	static int iter = 0;

	iter++;
//	if (iter > 1)
//	{
//		//printk("race !!! \n"); 
//	}
	DISABLE_PREEMPTION
////	SAVE_IF_STATUS
	STI
	while ((data_sckt_buf=dequeue_sckt(network_desc->rx_queue))!=NULL)
	{	
		CLI
		rcv_packet_mac(data_sckt_buf);
		i++;
		STI
//		if (system.force_scheduling != 0)
//		{
//			break;
//		}
	}
	CLI
////	RESTORE_IF_STATUS
	ENABLE_PREEMPTION
	iter--;
//	if (iter > 0)
//	{
//		//printk("race !!! \n"); 
//	}
}

void debug_network(char* data,u32 data_len)
{
	char* ip_payload;
        t_data_sckt_buf* data_sckt_buf=alloc_sckt(33+HEADER_ETH+HEADER_IP4+HEADER_UDP);
	data_sckt_buf->transport_hdr=data_sckt_buf->data+HEADER_ETH+HEADER_IP4;
	
	ip_payload=data_sckt_buf->transport_hdr+HEADER_UDP;
//	u32 src_ip=GET_DWORD(172,16,6,101);
//	u32 dst_ip=GET_DWORD(172,16,6,1);
	u32 src_ip=GET_DWORD(172,16,243,101);
	u32 dst_ip=GET_DWORD(172,16,243,1);
//	u32 dst_ip=GET_DWORD(95,246,209,232);

	kmemcpy(ip_payload,data,data_len);
	send_packet_udp(data_sckt_buf,src_ip,dst_ip,9999,45446,data_len);
}
