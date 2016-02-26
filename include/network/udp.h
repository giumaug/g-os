#ifndef UDP_H                
#define UDP_H

#include "network/network.h"

int send_packet_udp(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port,u16 data_len);
void rcv_packet_udp(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len);

#endif
