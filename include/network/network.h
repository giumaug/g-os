#ifndef NETWORK_H                
#define NETWORK_H

#include "system.h"
#include "network/common.h"
#include "network/network.h"---qui

typedef struct s_network_desc
{
	t_sckt_buf_desc* rx_queue;
	t_sckt_buf_desc* tx_queue;
	//WOULD BE BETTER TO ABSTRACT WITH A DEVICE
	t_i8254x* dev;

}
t_network_desc;

t_network_desc* network_init();
void network_free(t_network_desc* network_desc);
void equeue_packet(t_network_desc* network_desc);
void dequeue_packet(t_network_desc* network_desc);

#endif
