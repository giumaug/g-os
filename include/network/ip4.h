#ifndef IP4_H                
#define IP4_H

#include "network/network.h"

int send_packet_ip4(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,void* data,u16 data_len,u8 protocol);
void rcv_packet_ip4(t_data_sckt_buf* data_sckt_buf);

#endif
