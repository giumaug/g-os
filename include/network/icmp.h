#ifndef ICMP_H                
#define ICMP_H

#define ICMP_MAP_SIZE 20

struct s_data_sckt_buf;
 
typedef struct s_icmp_desc
{
	t_hashtable* req_map;
}
t_icmp_desc;

t_icmp_desc* icmp_init();
void icmp_free(t_icmp_desc* icmp_desc);
u32 _icmp_echo_request(u32 dst_ip,u16 icmp_msg_id,u16 icmp_msg_seq);
void rcv_packet_icmp(struct s_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len);

#endif
