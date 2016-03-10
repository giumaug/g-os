#include "network/network.h"

//ONLY ONE INSTANCE FOR ALL POSSIBLE INTERFACES!!!!!
t_network_desc* network_init()
{
	t_network_desc* network_desc;

	network_desc=kmalloc(sizeof(network_desc));
	network_desc->tx_queue=sckt_buf_desc_init();
	network_desc->rx_queue=sckt_buf_desc_init();
	network_desc->dev=init_8254x();
	network_desc->ip=LOCAL_IP;
	network_desc->netmask=LOCAL_NETMASK;
	//arp_init();
	return network_desc;
}

void network_free(t_network_desc* network_desc)
{
	sckt_buf_desc_free(network_desc->tx_queue);
	sckt_buf_desc_free(network_desc->rx_queue);
	free_8254x(network_desc->dev);
}

void equeue_packet(t_network_desc* network_desc)
{
	t_sckt_buf_desc* sckt_buf_desc;
	t_data_sckt_buf* data_sckt_buf;
	void* frame;
	u16 frame_len;

	sckt_buf_desc=network_desc->tx_queue;
	while ((data_sckt_buf=dequeue_sckt(sckt_buf_desc))!=NULL)
	{
		frame=data_sckt_buf->mac_hdr;
		frame_len=data_sckt_buf->data_len;
		send_packet_i8254x(network_desc->dev,frame,frame_len);
		sckt_buf_desc_free(sckt_buf_desc);
	}
}

void dequeue_packet(t_network_desc* network_desc)
{
	//t_sckt_buf_desc* sckt_buf_desc;
	t_data_sckt_buf* data_sckt_buf;

	while ((data_sckt_buf=dequeue_sckt(network_desc->rx_queue))!=NULL)
	{	
		rcv_packet_mac(data_sckt_buf);
	}
}
