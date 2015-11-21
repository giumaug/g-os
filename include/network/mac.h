#ifndef MAC_H                
#define MAC_H

#include "network/network.h"

void put_packet_mac(t_data_sckt_buf* data_sckt_buf,_mac_addr src_mac,_mac_addr dst_mac);
void rcv_packet_mac(data_sckt_buf);

#endif
