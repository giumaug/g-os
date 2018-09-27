#include "common.h"
#include "network/network.h"

t_hashtable* arp_cache=NULL;

void arp_init()
{
	arp_cache=hashtable_init(10);
}

void arp_free()
{
	hashtable_free(arp_cache);
	arp_cache=NULL;
}

t_mac_addr* lookup_mac(u32 dst_ip)
{
	struct t_process_context* current_process_context;
	t_mac_addr* mac_addr=NULL;
	t_mac_addr src_mac;
	t_mac_addr dst_mac;
	u32 src_ip;
	int i;

	mac_addr=hashtable_get(arp_cache,dst_ip);
	if (mac_addr==NULL)
	{
		src_mac=system.network_desc->dev->mac_addr;
		dst_mac.hi=0xFFFF;
		dst_mac.mi=0xFFFF;
		dst_mac.lo=0xFFFF;
		src_ip=system.network_desc->ip;

		for (i=0;i<ARP_ATTEMPT;i++)
		{
			printk("arp....\n");
			send_packet_arp(src_mac,dst_mac,src_ip,dst_ip,1);
			system.flush_network = 1;
			_sleep_time(ARP_REQUEST_TIMEOUT);
			mac_addr=hashtable_get(arp_cache,dst_ip);
			if (mac_addr!=NULL)
			{
				break;
			}
		}
	}
	return mac_addr;
}

void send_packet_arp(t_mac_addr src_mac,t_mac_addr dst_mac,u32 src_ip,u32 dst_ip,u8 op_type)
{
	unsigned char* arp_req=NULL;
	t_data_sckt_buf* data_sckt_buf=NULL;

	arp_req=kmalloc(MTU_ARP);

	arp_req[0]=HI_16(dst_mac.hi);       //BYTE 1 BROADCAST/DST MAC ADDRESS
	arp_req[1]=LOW_16(dst_mac.hi);      //BYTE 2 BROADCAST/DST MAC ADDRESS
	arp_req[2]=HI_16(dst_mac.mi);       //BYTE 3 BROADCAST/DST MAC ADDRESS
	arp_req[3]=LOW_16(dst_mac.mi);      //BYTE 4 BROADCAST/DST MAC ADDRESS
	arp_req[4]=HI_16(dst_mac.lo);       //BYTE 5 BROADCAST/DST MAC ADDRESS
	arp_req[5]=LOW_16(dst_mac.lo);      //BYTE 6 BROADCAST/DST MAC ADDRESS

	arp_req[6]=HI_16(src_mac.hi);       //BYTE 1 SRC MAC ADDRESS
	arp_req[7]=LOW_16(src_mac.hi);      //BYTE 2 SRC MAC ADDRESS
	arp_req[8]=HI_16(src_mac.mi);       //BYTE 3 SRC MAC ADDRESS
	arp_req[9]=LOW_16(src_mac.mi);      //BYTE 4 SRC MAC ADDRESS
	arp_req[10]=HI_16(src_mac.lo);      //BYTE 5 SRC MAC ADDRESS
	arp_req[11]=LOW_16(src_mac.lo);     //BYTE 6 SRC MAC ADDRESS

	arp_req[12]=0x8;                    //HIGH PROTOCOL TYPE
	arp_req[13]=0x6;                    //LOW PROTOCOL TYPE
	arp_req[14]=0x0;                    //HIGH HARD TYPE
	arp_req[15]=0x1;                    //LOW HARD TYPE
	arp_req[16]=0x8;                    //HIGH PORT TYPE
	arp_req[17]=0x0;                    //LOW PORT TYPE
    	arp_req[18]=0x6;                    //HARD SIZE
	arp_req[19]=0x4;                    //PORT SIZE
	arp_req[20]=0x0;                    //HIGH OP TYPE
	arp_req[21]=op_type;                //LOW OP TYPE

	arp_req[22]=HI_16(src_mac.hi);      //BYTE 1 SENDER MAC ADDRESS
	arp_req[23]=LOW_16(src_mac.hi);     //BYTE 2 SENDER MAC ADDRESS
	arp_req[24]=HI_16(src_mac.mi);      //BYTE 3 SENDER MAC ADDRESS
	arp_req[25]=LOW_16(src_mac.mi);     //BYTE 4 SENDER MAC ADDRESS
	arp_req[26]=HI_16(src_mac.lo);      //BYTE 5 SENDER MAC ADDRESS
	arp_req[27]=LOW_16(src_mac.lo);     //BYTE 6 SENDER MAC ADDRESS

	arp_req[28]=HI_OCT_32(src_ip);      //HIGH SENDER IP ADDRESS
	arp_req[29]=MID_LFT_OCT_32(src_ip); //MID LEFT SENDER IP ADDRESS
	arp_req[30]=MID_RGT_OCT_32(src_ip); //MID RIGHY SENDER IP ADDRESS
	arp_req[31]=LOW_OCT_32(src_ip);     //LOW SENDER IP ADDRESS

	arp_req[32]=HI_16(dst_mac.hi);      //BYTE 1 TARGET MAC ADDRESS
	arp_req[33]=LOW_16(dst_mac.hi);     //BYTE 2 TARGET MAC ADDRESS
	arp_req[34]=HI_16(dst_mac.mi);      //BYTE 3 TARGET MAC ADDRESS
	arp_req[35]=LOW_16(dst_mac.mi);     //BYTE 4 TARGET MAC ADDRESS
	arp_req[36]=HI_16(dst_mac.lo);      //BYTE 5 TARGET MAC ADDRESS
	arp_req[37]=LOW_16(dst_mac.lo);     //BYTE 6 TARGET MAC ADDRESS

	arp_req[38]=HI_OCT_32(dst_ip);      //HIGH TARGET IP ADDRESS
	arp_req[39]=MID_LFT_OCT_32(dst_ip); //MID LEFT TARGET IP ADDRESS
	arp_req[40]=MID_RGT_OCT_32(dst_ip); //MID RIGHY TARGET IP ADDRESS
	arp_req[41]=LOW_OCT_32(dst_ip);     //LOW TARGET IP ADDRESS

	arp_req[42]=0x0;                    //1  BYTE PAD
	arp_req[43]=0x0;                    //2  BYTE PAD
	arp_req[44]=0x0;                    //3  BYTE PAD
	arp_req[45]=0x0;                    //4  BYTE PAD
	arp_req[46]=0x0;                    //5  BYTE PAD
	arp_req[47]=0x0;                    //6  BYTE PAD
	arp_req[48]=0x0;                    //7  BYTE PAD
	arp_req[49]=0x0;                    //8  BYTE PAD
	arp_req[50]=0x0;                    //9  BYTE PAD
	arp_req[51]=0x0;                    //10 BYTE PAD
	arp_req[52]=0x0;                    //11 BYTE PAD
	arp_req[53]=0x0;                    //12 BYTE PAD
	arp_req[54]=0x0;                    //13 BYTE PAD
	arp_req[55]=0x0;                    //14 BYTE PAD
	arp_req[56]=0x0;                    //15 BYTE PAD
	arp_req[57]=0x0;                    //16 BYTE PAD
	arp_req[58]=0x0;                    //17 BYTE PAD
	arp_req[59]=0x0;                    //18 BYTE PAD

	data_sckt_buf=alloc_void_sckt();
	data_sckt_buf->mac_hdr=arp_req;
	data_sckt_buf->data=arp_req;
	data_sckt_buf->data_len=MTU_ARP;
	enqueue_sckt(system.network_desc->tx_queue,data_sckt_buf);	
}

void rcv_packet_arp(t_data_sckt_buf* data_sckt_buf)
{
	u32 dst_ip;
	u32 src_ip;
	t_mac_addr dst_mac;
	t_mac_addr src_mac;
	t_mac_addr* mac_to_cache;
	unsigned char* arp_rsp;

	arp_rsp=data_sckt_buf->mac_hdr;
	u16 optype=arp_rsp[21];
	
	dst_ip=GET_DWORD(arp_rsp[38],arp_rsp[39],arp_rsp[40],arp_rsp[41]);
	src_ip=GET_DWORD(arp_rsp[28],arp_rsp[29],arp_rsp[30],arp_rsp[31]);
	
	dst_mac.hi=GET_WORD(arp_rsp[32],arp_rsp[33]);
	dst_mac.mi=GET_WORD(arp_rsp[34],arp_rsp[35]);
	dst_mac.lo=GET_WORD(arp_rsp[36],arp_rsp[37]);

	src_mac.hi=GET_WORD(arp_rsp[22],arp_rsp[23]);
	src_mac.mi=GET_WORD(arp_rsp[24],arp_rsp[25]);
	src_mac.lo=GET_WORD(arp_rsp[26],arp_rsp[27]);

 	if (optype==1)
	{
		if (dst_ip==system.network_desc->ip)
		{
			dst_mac=system.network_desc->dev->mac_addr;
			send_packet_arp(dst_mac,src_mac,dst_ip,src_ip,2);
		}
	}
	else if (optype==2)
	{
		mac_to_cache=hashtable_get(arp_cache,src_ip);
		if (mac_to_cache==NULL)
		{
			mac_to_cache=kmalloc(sizeof(t_mac_addr));
			*mac_to_cache=src_mac;
			hashtable_put(arp_cache,src_ip,mac_to_cache);
		}
	}

	free_sckt(data_sckt_buf);
}
