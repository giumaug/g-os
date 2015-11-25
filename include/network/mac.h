#ifndef MAC_H                
#define MAC_H

#include "network/network.h"

void put_packet_mac(t_data_sckt_buf* data_sckt_buf,t_mac_addr src_mac,t_mac_addr dst_mac);
void rcv_packet_mac(t_data_sckt_buf* data_sckt_buf);

#endif
