#ifndef NETWORK_H                
#define NETWORK_H

#include "system.h"
#include "data_types/queue.h"
#include "drivers/i8254x/i8254x.h"

#define NETWORK_PROTOCOL_TYPE 0x800
#define MTU_IP4 65536
//PARTIAMO DALL'IPOTESI 1518 BYTES FRAME CON 14 HEADER BYTES.SUPPONGO PURE CHE PREAMBOLO E CRC E' INSERITO DALL'HARDWARE.
#define MTU_ETH 1518
#define HEADER_IP4 20
#define HEADER_ETH 14
#define HEADER_UDP 8
#define HEADER_TCP
#define SOCKET_BUFFER_SIZE 2000
#define TCP_PROTOCOL 	4
#define UDP_PROTOCOL 	6
#define ICMP_PROTOCOL	1
#define LOCAL_IP	0xC0A8010A
#define LOCAL_NETMASK   0xFFFFFF00
   
#define LOW_32(data)		(data & 0xFFFF)
#define HI_32(data)		((data>>16) & 0xFFFF)
#define LOW_16(data) 		(data & 0xFF)
#define HI_16(data)  		((data>>8) & 0xFF)
#define IP_LOW_OCT(data)	(data & 0xFF)
#define IP_MID_RGT_OCT(data)	(data>>8 && 0xFF)
#define IP_MID_LFT_OCT(data)	(data>>16 && 0xFF)
#define IP_HI_OCT(data)		(data>>24 && 0xFF)

typedef struct s_data_sckt_buf
{
	unsigned char* transport_hdr;
	unsigned char* network_hdr;
	unsigned char* mac_hdr;
	unsigned char* head;		//start protocol memory area
	unsigned char* end;    		//end protocol memory area
	unsigned char* data;   		//start data memory area
	unsigned char* tail;   		//end data memory area
}
t_data_sckt_buf;

typedef struct s_sckt_buf_desc
{
	t_queue* buf;
	u32 buf_size;
	u32 buf_index;
	t_spinlock_desc lock;
}
t_sckt_buf_desc;

typedef struct s_mac_addr
{
	u16 lo;
	u16 mi;
	u16 hi;
}
t_mac_addr;

struct s_i8254x;

typedef struct s_network_desc
{
	t_sckt_buf_desc* rx_queue;
	t_sckt_buf_desc* tx_queue;
	//WOULD BE BETTER TO ABSTRACT WITH A DEVICE
	struct s_i8254x* dev;

}
t_network_desc;

t_network_desc* network_init();
void network_free(t_network_desc* network_desc);
void equeue_packet(t_network_desc* network_desc);
void dequeue_packet(t_network_desc* network_desc);

t_sckt_buf_desc* sckt_buf_desc_init();
void sckt_buf_desc_free(t_sckt_buf_desc* sckt_buf_desc);
void enqueue_sckt(t_sckt_buf_desc* sckt_buf_desc,t_data_sckt_buf* data_sckt_buf);
t_data_sckt_buf* dequeue_sckt(t_sckt_buf_desc* sckt_buf_desc);
t_data_sckt_buf* alloc_sckt(u16 data_len);
void free_sckt(t_data_sckt_buf* data_sckt_buf);

void put_packet_mac(t_data_sckt_buf* data_sckt_buf,t_mac_addr src_mac,t_mac_addr dst_mac);
void rcv_packet_mac(t_data_sckt_buf* data_sckt_buf);

int send_packet_ip4(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,void* data,u16 data_len,u8 protocol);
void rcv_packet_ip4(t_data_sckt_buf* data_sckt_buf);

t_mac_addr lookup_mac(u32 ip_addr);

#endif
