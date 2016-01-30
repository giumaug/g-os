#ifndef UDP_H                
#define UDP_H

#include "network/network.h"

int send_packet_udp(t_data_sckt_buf* data_sckt_buf,u16 src_port,u16 dst_port,u16 data_len);

#endif
