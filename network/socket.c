#define SOCKET_MAP_SIZE 100
#define TCP_MAP_SIZE 20
#define UDP_MAP_SIZE 20

typedef struct s_socket_desc
{
	t_hashtable* sd_map=NULL;
	t_hashtable* tcp_map=NULL;
	t_hashtable* udp_map=NULL;
	u32 udp_port_indx;
	u32 tcp_port_indx;
	u32 fd=0;
}
t_socket_desc;

typedef struct s_socket_addr
{
	u32 ip;
	u32 src_port;
	u32 dst_port;
	u32 protocol;
}
t_socket_addr;

static t_socket_desc socket_desc;

void socket_init()
{
	socket_desc.sd_map=hashtable_init(SOCKET_MAP_SIZE);
	socket_desc.tcp_map=hashtable_init(TCP_MAP_SIZE);
	socket_desc.udp_map=hashtable_init(UDP_MAP_SIZE);
}

void socket_free()
{
	hashtable_free(socket_desc.sd_map);
	hashtable_free(socket_desc.tcp_map);
	hashtable_free(socket_desc.udp_map);
	socket_desc.sd_map=NULL;
	socket_desc.tcp_map=NULL;
	socket_desc.udp_map=NULL;

int _socket(u32 ip, u32 port, int protocol) 
{
	t_socket_addr socket_addr=NULL;

	socket_addr=kmalloc(sizeof(t_socket_addr));
	socket_addr->protocol=protocol;
	socket_desc.fd++;
	hashtable_put(socket_desc.sd_map,socket_desc.fd,socket_addr);
	return socket_desc.fd;
}

int _bind(int sockfd,u32 ip,u32 port)
{
	t_socket_addr socket_addr=NULL;
	u32 src_port=NULL;

	socket_addr=hashtable_get(socket_desc.sd_map,sockfd);
	if (socket_addr!=NULL)
	{
		if (protocol==2)
		{
ù			for (i=0;i<32767;i++)
			{
				socket_desc.udp_map++;
				src_port=32768+(socket_desc.udp_map & 0x7FFF ----------------qui!!!!
			}
		}
		socket_desc.udp_map++;
		socket_addr->ip=ip;
		socket_addr->dst_port=port;
		socket_addr->src_port=
	}
}

ssize_t _recvfrom(int sockfd, void *buf, size_t len, int flags,struct sockaddr *src_addr, socklen_t *addrlen)
{

}

ssize_t _sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen)
{
	char* ip_payload=NULL;
        t_data_sckt_buf* data_sckt_buf=NULL;



	data_sckt_buf=alloc_sckt(33+HEADER_ETH+HEADER_IP4+HEADER_UDP);
	data_sckt_buf->transport_hdr=data_sckt_buf->data+HEADER_ETH+HEADER_IP4;
	
	ip_payload=data_sckt_buf->transport_hdr+HEADER_UDP;
//	u32 src_ip=GET_DWORD(172,16,6,101);
//	u32 dst_ip=GET_DWORD(172,16,6,1);
	u32 src_ip=GET_DWORD(172,16,243,101);
	u32 dst_ip=GET_DWORD(172,16,243,1);
//	u32 dst_ip=GET_DWORD(95,246,209,232);

	kmemcpy(ip_payload,data,data_len);
	send_packet_udp(data_sckt_buf,src_ip,dst_ip,9999,45446,data_len);


}

close ???
