#include "common.h"
#include "system.h"
#include "network/icmp.h"

static void send_packet_icmp_reply(u32 src_ip,u32 dst_ip,char* icmp_row_packet,u16 packet_len);
static void send_packet_icmp_request(u32 src_ip,u32 dst_ip,u16 icmp_msg_id,u16 icmp_msg_seq);

t_icmp_desc* icmp_init()
{
	t_icmp_desc* icmp_desc = NULL;

	icmp_desc = kmalloc(sizeof(t_icmp_desc));
	icmp_desc->req_map = hashtable_init(ICMP_MAP_SIZE);
	return icmp_desc;
}

void icmp_free(t_icmp_desc* icmp_desc)
{
	hashtable_free(icmp_desc->req_map);
	kfree(icmp_desc);
}

u32 _icmp_echo_request(u32 dst_ip,u16 icmp_msg_id,u16 icmp_msg_seq)
{
	u32 key,t1,t2;
	t_icmp_desc* icmp_desc = NULL;
	struct t_process_context* process_context = NULL;

	SAVE_IF_STATUS
	CLI
	CURRENT_PROCESS_CONTEXT(process_context);
	icmp_desc = system.network_desc->icmp_desc;	
	key = (icmp_msg_id << 16) | icmp_msg_seq;
	hashtable_put(icmp_desc->req_map,key,process_context);
	process_context->icmp_pending_req = key;
	t1 = system.time;
	send_packet_icmp_request(system.network_desc->ip,dst_ip,icmp_msg_id,icmp_msg_seq);
	_sleep();
	t2 = system.time;
	RESTORE_IF_STATUS
	return t2 - t1;
}

void rcv_packet_icmp(t_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len)
{
	u32 key;
	u16 icmp_msg_id,icmp_msg_seq;
	unsigned char* icmp_row_packet;
	struct t_process_context* process_context = NULL;
	u16 packet_len;

	icmp_row_packet = data_sckt_buf->transport_hdr;
	packet_len = HEADER_ICMP + data_len;
	if (checksum((unsigned short*) icmp_row_packet,packet_len) == 0)
	{
		switch((u8) icmp_row_packet[0])
		{
			case 0: //Echo reply
			{
				icmp_msg_id = GET_WORD(icmp_row_packet[4],icmp_row_packet[5]);
				icmp_msg_seq = GET_WORD(icmp_row_packet[6],icmp_row_packet[7]);
				key = (icmp_msg_id << 16) | icmp_msg_seq;
				process_context = hashtable_remove(system.network_desc->icmp_desc->req_map,key);
				if (process_context != NULL)
				{
					process_context->icmp_pending_req = NULL;
					printk("...\n");
					_awake(process_context);
				}
				break;
			}
			case 8: //Echo request
			{
				packet_len = HEADER_ICMP + data_len;
				icmp_msg_id = GET_WORD(icmp_row_packet[4],icmp_row_packet[5]);
				icmp_msg_seq = GET_WORD(icmp_row_packet[6],icmp_row_packet[7]);
				send_packet_icmp_reply(dst_ip,src_ip,icmp_row_packet,packet_len);
				break;
			}
		}
	}
}

static void send_packet_icmp_reply(u32 src_ip,u32 dst_ip,char* icmp_row_packet,u16 packet_len)
{
	t_data_sckt_buf* data_sckt_buf = NULL;
	u16 chk = 0;
	
	data_sckt_buf = alloc_sckt(60 + HEADER_ETH + HEADER_IP4 + HEADER_ICMP);
	data_sckt_buf->transport_hdr = data_sckt_buf->data + HEADER_ETH + HEADER_IP4;
	data_sckt_buf->network_hdr = data_sckt_buf->transport_hdr - HEADER_IP4;
	icmp_row_packet[0] = 0;
	icmp_row_packet[2] = 0;
	icmp_row_packet[3] = 0;
	chk = SWAP_WORD(checksum(icmp_row_packet,packet_len));
	icmp_row_packet[2] = HI_16(chk);
	icmp_row_packet[3] = LOW_16(chk);
	kmemcpy(data_sckt_buf->transport_hdr,icmp_row_packet,packet_len);
	send_packet_ip4(data_sckt_buf,src_ip,dst_ip,packet_len,ICMP_PROTOCOL);
}

static void send_packet_icmp_request(u32 src_ip,u32 dst_ip,u16 icmp_msg_id,u16 icmp_msg_seq)
{
	int ret = 0;
	u16 chk = 0;
	unsigned char* icmp_row_packet = NULL;
	u16 ip_packet_len = 0;
	t_data_sckt_buf* data_sckt_buf = NULL;
	char* ip_payload = NULL;

	data_sckt_buf = alloc_sckt(60 + HEADER_ETH + HEADER_IP4 + HEADER_ICMP);
	data_sckt_buf->transport_hdr = data_sckt_buf->data + HEADER_ETH + HEADER_IP4;
	ip_payload = data_sckt_buf->transport_hdr + HEADER_ICMP;
	ip_payload[0] = HI_16(icmp_msg_id);              
 	ip_payload[1] = LOW_16(icmp_msg_id);    
	ip_payload[2] = HI_16(icmp_msg_seq);  
	ip_payload[3] = LOW_16(icmp_msg_seq); 
	ip_payload[4] = 0;
	ip_payload[5] = 0;
	ip_payload[6] = 0;
	ip_payload[7] = 0;
	ip_payload[8] = 0;
	ip_payload[9] = 0;
	ip_payload[10] = 0;
	ip_payload[11] = 0;
	ip_payload[12] = 0;
	ip_payload[13] = 0;
	ip_payload[14] = 0;
	ip_payload[15] = 0;
	ip_payload[16] = 0;
	ip_payload[17] = 0;
	ip_payload[18] = 0;
	ip_payload[19] = 0;
	ip_payload[20] = 0;
	ip_payload[21] = 0;
	ip_payload[22] = 0;
	ip_payload[23] = 0;
	ip_payload[24] = 0;
	ip_payload[25] = 0;
	ip_payload[26] = 0;
	ip_payload[27] = 0;
	ip_payload[28] = 0;
	ip_payload[29] = 0;
	ip_payload[30] = 0;
	ip_payload[31] = 0;
	ip_payload[32] = 0;
	ip_payload[33] = 0;
	ip_payload[34] = 0;
	ip_payload[35] = 0;
	ip_payload[36] = 0;
	ip_payload[37] = 0;
	ip_payload[38] = 0;
	ip_payload[39] = 0;
	ip_payload[40] = 0;
	ip_payload[41] = 0;
	ip_payload[42] = 0;
	ip_payload[43] = 0;
	ip_payload[44] = 0;
	ip_payload[45] = 0;
	ip_payload[46] = 0;
	ip_payload[47] = 0;
	ip_payload[48] = 0;
	ip_payload[49] = 0;
	ip_payload[50] = 0;
	ip_payload[51] = 0;
	ip_payload[52] = 0;
	ip_payload[53] = 0;
	ip_payload[54] = 0;
	ip_payload[55] = 0;
	
	icmp_row_packet = data_sckt_buf->transport_hdr;
	data_sckt_buf->network_hdr = data_sckt_buf->transport_hdr - HEADER_IP4;
	ip_packet_len = HEADER_ICMP + 60; 
	
	icmp_row_packet[0] = 8;           
	icmp_row_packet[1] = 0;              
	icmp_row_packet[2] = 0;
     	icmp_row_packet[3] = 0;         
	chk = SWAP_WORD(checksum(icmp_row_packet,64));
	icmp_row_packet[2] = HI_16(chk);
	icmp_row_packet[3] = LOW_16(chk);
	send_packet_ip4(data_sckt_buf,src_ip,dst_ip,ip_packet_len,ICMP_PROTOCOL);
	return ret;
}
