#ifndef NETWORK_H                
#define NETWORK_H

#include "system.h"
#include "data_types/queue.h"
#include "network/common.h"
#include "network/socket_buffer.h"
#include "drivers/i8254x/i8254x.h"

struct s_i8254x;
struct s_sckt_buf_desc;

typedef struct s_network_desc
{
	struct s_sckt_buf_desc* rx_queue;
	struct s_sckt_buf_desc* tx_queue;
	//WOULD BE BETTER TO ABSTRACT WITH A DEVICE
	struct s_i8254x* dev;

}
t_network_desc;

t_network_desc* network_init();
void network_free(t_network_desc* network_desc);
void equeue_packet(t_network_desc* network_desc);
void dequeue_packet(t_network_desc* network_desc);

#endif
