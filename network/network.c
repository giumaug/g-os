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
	arp_init();
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
		printk("dddddddd \n");
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

void debug_network()
{
	char* ip_payload;
        t_data_sckt_buf* data_sckt_buf=alloc_sckt(33+HEADER_ETH+HEADER_IP4+HEADER_UDP);
	data_sckt_buf->transport_hdr=data_sckt_buf->data+HEADER_ETH+HEADER_IP4;
	//enqueue_sckt(system.network_desc->tx_queue,data_sckt_buf);
	
	char data[]="This.is.a.fake.udp.packet!!!!!!!!";
	ip_payload=data_sckt_buf->transport_hdr+HEADER_UDP;
	u32 src_ip=GET_DWORD(172,16,6,101);
	u32 dst_ip=GET_DWORD(172,16,6,1);
//	u32 src_ip=GET_DWORD(172,16,243,101);
//	u32 dst_ip=GET_DWORD(172,16,243,1);

	kmemcpy(ip_payload,data,33);
	send_packet_udp(data_sckt_buf,src_ip,dst_ip,9999,9999,33);
	printk("dd \n");
	//equeue_packet(system.network_desc);
}
