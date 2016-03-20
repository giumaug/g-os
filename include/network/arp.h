#ifndef ARP_H                
#define ARP_H

#include "network/network.h"
#include "network/mac.h"

t_mac_addr* lookup_mac(u32 dst_ip);
void send_packet_arp(t_mac_addr src_mac,t_mac_addr dst_mac,u32 src_ip,u32 dst_ip,u8 op_type);
void rcv_packet_arp(t_data_sckt_buf* data_sckt_buf);

#endif
