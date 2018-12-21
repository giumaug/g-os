#include "common.h"
#include "network/network.h"

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
	//int stop;
	t_sckt_buf_desc* sckt_buf_desc;
	t_data_sckt_buf* data_sckt_buf;
	void* frame;
	u16 frame_len;
	u32 data_sent = 0;
	static u32 tot_sent = 0;
	int x = 0;
	
//	DISABLE_PREEMPTION
//	STI 
	sckt_buf_desc=network_desc->tx_queue;
	while ((data_sckt_buf=dequeue_sckt(sckt_buf_desc))!=NULL)
	{
		x++;
		frame=data_sckt_buf->mac_hdr;
		frame_len=data_sckt_buf->data_len;
		//CLI
		send_packet_i8254x(network_desc->dev,frame,frame_len);
		//STI
		free_sckt(data_sckt_buf);
	}
	if (x > 0)
	{//system.flush_network = 1;
		//printk("inside enqueue....%d \n",x);
	}
//	CLI
//	ENABLE_PREEMPTION
}

void dequeue_packet(t_network_desc* network_desc)
{
	t_data_sckt_buf* data_sckt_buf;
	int x = 0;

	DISABLE_PREEMPTION
	STI
	while ((data_sckt_buf=dequeue_sckt(network_desc->rx_queue))!=NULL)
	{
		//printk("deq ");
		x++;	
		CLI 
		rcv_packet_mac(data_sckt_buf);
		STI
		if (system.force_scheduling != 0)
		{
		//	break;
		}
		if (x >= 3)
		{
			system.flush_network = 1;
			break;
		}
		else 
		{
			system.flush_network = 0;	
		}
	}
	system.flush_network = 1;
	if (x > 0)
	{
		//printk("deq-end ");
	}
	CLI
	ENABLE_PREEMPTION
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
