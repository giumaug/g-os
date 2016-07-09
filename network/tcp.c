#define TCP_RCV_SIZE 
#define TCP_SND_SIZE

#define RCV_PORT_MAP_SIZE 	20
#define RCV_ACK_PORT_MAP_SIZE 	20
#define SND_PORT_MAP_SIZE 	20

#define INC_WND(cur,wnd_size,offset)  (cur + offset) % wnd_size

typedef t_data_sckt_buf t_tcp_rcv_buf;
typedef t_data_sckt_buf s_tcp_snd_buf;


typedef struct s_tcp_queue
{
	u32 min;
	u32 max;
	u32 cur;
	u32 size;
	t_tcp_rcv_buf* buf;
}
t_tcp_queue

t_tcp_queue tcp_queue_init(u32 size)
{
	int i;
	t_tcp_queue* tcp_queue;

	tcp_queue->buf=kmalloc(sizeof(t_data_sckt_buf*)*size);
	for (i=0;i<size;i++)
	{
		tcp_queue->buf[i]=NULL;
	}
	tcp_queue->min=0;
	tcp_queue->max=0;
	tcp_queue->cur=0;
	tcp_queue->size=size;
	return tcp_queue;
}

int tcp_queue_add(t_tcp_queue* tcp_queue,t_data_sckt_buf* data_sckt_buf)
{
	if (INC_WND(tcp_queue->cur,tcp_queue->size,1))
	{
		return -1;
	}
	tcp_queue->cur=INC_WND(tcp_queue->cur,tcp_queue->size,1);
	tcp_queue->buf[tcp_queue->cur]=data_sckt_buf;
	return 0;
}

tcp_queue_del(t_tcp_queue* tcp_queue,t_data_sckt_buf* data_sckt_buf)
{
-----
}



typedef struct s_tcp_conn_desc
{
	u16 src_port;
	u16 dst_port;
	t_tcp_rcv_buf* rcv_buf;
	t_tcp_snd_buf* snd_buf;
}
t_tcp_conn_desc;

void tcp_conn_desc_int(u16 src_port,u16 dst_port)
{
	t_tcp_conn_desc tcp_conn_desc;

	tcp_conn_desc=kmalloc(sizeof(t_tcp_conn_desc));
	tcp_conn_desc->rcv_buf=kmalloc(sizeof(t_data_sckt_buf*)*TCP_RCV_SIZE); //attenzione array di puntatori
	tcp_conn_desc->snd_buf=kmalloc(sizeof(t_data_sckt_buf*)*TCP_SND_SIZE); //attenzione array di puntatori
	tcp_conn_desc->src_port=src_port;
	tcp_conn_desc->dst_port=dst_port;
}



//IMPORTANTE VERIFICARE CHE LA STRUTTURA DATI COMPLESSIVA SIA SENSATA!!!!!!!!!!!!!!!!!!



typedef struct s_tcp_desc
{
	t_hashtable* rcv_port_map;
}
t_tcp_desc

void tcp_init()
{
	t_tcp_desc tcp_desc;
	
	tcp_desc=kmalloc(sizeof(t_tcp_desc));
	tcp_desc->rcv_port_map=hashtable_init(RCV_PORT_MAP_SIZE);
	tcp_desc->snd_port_map=hashtable_init(SND_PORT_MAP_SIZE);
	tcp_desc->rcv_ack_port_map=hashtable_init(RCV_ACK_PORT_MAP_SIZE);
}

void process_rcv_packet()
{

}

void process_snd_packet()
{

}


void rcv_packet_tcp(t_data_sckt_buf* data_sckt_buf)
{
	if (ack) 
	{
		add ack queue
	}
	else if (packet data)
	{
		add rcv queue
	}
}

void snd_packet_tcp(char* data,int data_len........)
{
	char* tcp_payload;
	t_data_sckt_buf* data_sckt_buf;
	
	data_sckt_buf=alloc_sckt(data_len+HEADER_ETH+HEADER_IP4+HEADER_TCP);
	data_sckt_buf->transport_hdr=data_sckt_buf->data+HEADER_ETH+HEADER_IP4;
	tcp_payload=data_sckt_buf->transport_hdr+HEADER_TCP;
	kmemcpy(tcp_payload,data,data_len);
	tcp_header=data_sckt_buf->transport_hdr;
	
	tcp_header[0]=HI_16(src_port);   		//HI SRC PORT
	tcp_header[1]=LOW_16(src_port);  		//LOW SRC PORT
	tcp_header[2]=HI_16(src_port);   		//HI DST PORT
	tcp_header[3]=LOW_16(src_port);  		//LOW DST PORT

	tcp_header[4]=HI_OCT_32(seq_num);         	//HIGH SEQ NUM
	tcp_header[5]=MID_LFT_OCT_32(seq_num);    	//MID LEFT SEQ NUM
	tcp_header[6]=MID_RGT_OCT_32(seq_num);    	//MID RIGHT SEQ NUM
	tcp_header[7]=LOW_OCT_32(seq_num);        	//LOW SEQ NUM

	tcp_header[8]=HI_OCT_32(ack_num);		//HIGH ACK NUM
	tcp_header[9]=MID_LFT_OCT_32(ack_num);		//MID LEFT ACK NUM
	tcp_header[10]=MID_RGT_OCT_32(ack_num); 	//MID RIGHT ACK NUM
	tcp_header[11]=LOW_OCT_32(ack_num);		//LOW ACK NUM

	tcp_header[12]=head_len << 4;                   //HEADER LEN + 4 RESERVED BIT
	tcp_header[13]=flags;                           //FLAGS
	tcp_header[14]=HI_16(win_size);			//HI WINDOW SIZE
	tcp_header[15]=LOW_16(win_size);                //LOW WINDOW SIZE

	tcp_header[16]=HI_16(checksum);			//HI TCP CHECKSUM
	tcp_header[17]=LOW_16(checksum);		//LOW TCP CHECKSUM
	tcp_header[18]=0;				//HI URGENT POINTER (NOT USED)
	tcp_header[19]=0;				//LOW URGENT POINTER (NOT USED)
}


