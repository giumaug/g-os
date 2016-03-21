#ifndef COMMON_H                
#define COMMON_H

#define ARP_REQUEST_TIMEOUT   2000
#define ARP_ATTEMPT 	      3
#define NETWORK_PROTOCOL_TYPE 0x800
#define ARP_PROTOCOL_TYPE     0x806
#define MTU_IP4 65536
//PARTIAMO DALL'IPOTESI 1518 BYTES FRAME CON 14 HEADER BYTES.SUPPONGO PURE CHE PREAMBOLO E CRC E' INSERITO DALL'HARDWARE.
#define MTU_ETH 1518
#define MTU_ARP 60
#define HEADER_IP4 20
#define HEADER_ETH 14
#define HEADER_UDP 8
#define HEADER_TCP
#define SOCKET_BUFFER_SIZE 2000
#define TCP_PROTOCOL 	0x6
#define UDP_PROTOCOL 	0x11
#define ICMP_PROTOCOL	1
//#define LOCAL_IP	0xAC10F365
#define LOCAL_IP        0xAC100665
#define LOCAL_NETMASK   0xFFFFFF00
   
#define LOW_32(data)			(data & 0xFFFF)
#define HI_32(data)			((data>>16) & 0xFFFF)
#define LOW_16(data) 			(data & 0xFF)
#define HI_16(data)  			((data>>8) & 0xFF)
#define IP_LOW_OCT(data)		(data & 0xFF)
#define IP_MID_RGT_OCT(data)		(data>>8 & 0xFF)
#define IP_MID_LFT_OCT(data)		(data>>16 & 0xFF)
#define IP_HI_OCT(data)			(data>>24 & 0xFF)
//#define IP_FROM_OCT_TO_LONG(a,b,c,d) 	(d | (c<<8) | (b<<16) | (a<<24))
#define SWAP_WORD(data)           	(data>>8) | (data<<8)
#define GET_WORD(a,b)			(b | (a<<8))
#define GET_DWORD(a,b,c,d)		(d | (c<<8) | (b<<16) | (a<<24))

typedef struct s_mac_addr
{
	u16 lo;
	u16 mi;
	u16 hi;
}
t_mac_addr;

#endif
