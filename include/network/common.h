#ifndef COMMON_NETWORK_H                
#define COMMON_NETWORK_H

#define HOME_PC
//#define VMWARE
//#define HOME_PC
//#define WORK_PC
//#define AMAZON

#define ARP_REQUEST_TIMEOUT   200
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
#define HEADER_TCP 20
#define HEADER_ICMP 4
#define SOCKET_BUFFER_SIZE 2000
#define TCP_PROTOCOL 	0x6
#define UDP_PROTOCOL 	0x11
#define ICMP_PROTOCOL	0x1

#ifdef HOME_PC
#define LOCAL_IP 	    0xC0A80541
#define LOCAL_NETMASK   0xFFFFFF00
#define DEFAULT_GW_IP 	0xC0A80501
#endif

#ifdef KVM
#define LOCAL_IP	    0xC0A87A65
#define LOCAL_NETMASK   0xFFFFFF00
#define DEFAULT_GW_IP   0xC0A87A02
#endif

#ifdef VMWARE
#define LOCAL_IP	    0xC0A8F765
#define LOCAL_NETMASK   0xFFFFFF00
#define DEFAULT_GW_IP   0xC0A8F702
#endif

#ifdef WORK_PC
#define LOCAL_IP	    0xC0A8ED65
#define LOCAL_NETMASK   0xFFFFFF00
#define DEFAULT_GW_IP   0xC0A8ED02
#endif

#ifdef AMAZON
#define LOCAL_IP	    0xAC100665  
#define LOCAL_NETMASK   0xFFFFFF00
#define DEFAULT_GW_IP   0xAC100664
#endif

typedef struct s_mac_addr
{
	u16 lo;
	u16 mi;
	u16 hi;
}
t_mac_addr;

#endif
