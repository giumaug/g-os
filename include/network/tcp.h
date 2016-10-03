#define SMSS 1454
#define TCP_RCV_SIZE 
#define TCP_SND_SIZE
#define TCP_CONN_MAP_SIZE 	20

#define INC_WND(cur,wnd_size,offset)  (cur + offset) % wnd_size
#define SLOT_WND (cur,wnd_size) (cur % wnd_size
#define DATA_IN_WND(min,max,index)				\
(								\
	(min <= max) ?						\
	(							\
		(index >= min && index <= max) ? 1 : 0		\
	)							\
	:							\
	(							\
		(index >= max && index <= min) ? 1 : 0		\
	)							\
)								\

#define DATA_LF_OUT_WND(min,max,index) ((index < min) ? 1 : 0)
#define DATA_RH_OUT_WND(min,max,index) ((index > min) ? 1 : 0)

#define FLG_CWR 0b1000000
#define FLG_ECE 0b0100000
#define FLG_URG 0b0010000
#define FLG_ACK 0b0001000
#define FLG_PSH 0b0000100
#define FLG_RST 0b0000010
#define FLG_SYN 0b0000001

//timout=200 ms (quantum = 10 ms)
#define PIGGYBACKING_TIMEOUT = 20 

typedef struct s_tcp_snd_queue
{
	u32 buf_min; //equal to wnd_min
	u32 buf_max;
	u32 wnd_min;
	u32 wnd_max;
	u32 buf_cur;
	u32 buf_size;
	char* buf;
	u32 nxt_snd;
}
t_tcp_snd_queue;

typedef struct s_tcp_rcv_queue
{
	u32 wnd_min;
	u32 wnd_max;
	u32 buf_size;
	char* buf;
	char* buf_status;
	u32 nxt_rcv; //ack relativo finestra di ricezione e usato in trasmissione
	u32 wnd_size;
}
t_tcp_rcv_queue;

typedef struct s_tcp_conn_desc
{
	u32 conn_id;
	u32 rto;
	u32 rtrsn_timer;
	u32 pgybg_timer;
	t_tcp_rcv_queue* rcv_queue;
	t_tcp_snd_queue* snd_queue;
	u32 seq_num;
	u32 ack_seq_num;
//	u32 offered_ack; //va usato nxt_rcv
//	u32 expected_ack; // va usato nxt_snd
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
}
t_tcp_conn_desc;

typedef struct s_tcp_desc
{
	t_hashtable* conn_map;
	t_llist* conn_list;
	new_dllist tcp_conn_desc_free(t_tcp_conn_desc* tcp_conn_desc)
}
t_tcp_desc;

t_tcp_snd_queue* tcp_snd_queue_init(u32 size)
{
	t_tcp_snd_queue* tcp_snd_queue;

	tcp_snd_queue=kmalloc(sizeof(t_tcp_snd_queue));
	tcp_snd_queue->buf=kmalloc(size);
	tcp_snd_queue->wnd_min=0;
	tcp_snd_queue->wnd_max=0;
	tcp_snd_queue->buf_cur=0;
	tcp_snd_queue->buf_size=size;
	tcp_snd_queue->nxt_snd=0;
	return tcp_snd_queue;
}

u8 lookup_reserved_slot[126];
lookup_reserved_slot[0] =  1;  	// 10000000
lookup_reserved_slot[1] =  1;  	// 10000001
lookup_reserved_slot[2] =  1;  	// 10000010
lookup_reserved_slot[3] =  1;  	// 10000011
lookup_reserved_slot[4] =  1;  	// 10000100
lookup_reserved_slot[5] =  1;  	// 10000101
lookup_reserved_slot[6] =  1;  	// 10000110
lookup_reserved_slot[7] =  1;  	// 10000111
lookup_reserved_slot[8] =  1;  	// 10001000
lookup_reserved_slot[9] =  1;  	// 10001001
lookup_reserved_slot[10] =  1;
lookup_reserved_slot[11] =  1;
lookup_reserved_slot[12] =  1;
lookup_reserved_slot[13] =  1;
lookup_reserved_slot[14] =  1;
lookup_reserved_slot[15] =  1;
lookup_reserved_slot[16] =  1;
lookup_reserved_slot[17] =  1;
lookup_reserved_slot[18] =  1;
lookup_reserved_slot[19] =  1;
lookup_reserved_slot[20] =  1;
lookup_reserved_slot[21] =  1;
lookup_reserved_slot[22] =  1;
lookup_reserved_slot[23] =  1;
lookup_reserved_slot[24] =  1;
lookup_reserved_slot[25] =  1;
lookup_reserved_slot[26] =  1;
lookup_reserved_slot[27] =  1;
lookup_reserved_slot[28] =  1;
lookup_reserved_slot[29] =  1;

lookup_reserved_slot[30] =  1;
lookup_reserved_slot[31] =  1;
lookup_reserved_slot[32] =  1;
lookup_reserved_slot[33] =  1;
lookup_reserved_slot[34] =  1;
lookup_reserved_slot[35] =  1;
lookup_reserved_slot[36] =  1;
lookup_reserved_slot[37] =  1;
lookup_reserved_slot[38] =  1;
lookup_reserved_slot[39] =  1;
lookup_reserved_slot[40] =  1;
lookup_reserved_slot[41] =  1;
lookup_reserved_slot[42] =  1;
lookup_reserved_slot[43] =  1;
lookup_reserved_slot[44] =  1;
lookup_reserved_slot[45] =  1;
lookup_reserved_slot[46] =  1;
lookup_reserved_slot[47] =  1;
lookup_reserved_slot[48] =  1;
lookup_reserved_slot[49] =  1;
lookup_reserved_slot[50] =  1;
lookup_reserved_slot[51] =  1;
lookup_reserved_slot[52] =  1;
lookup_reserved_slot[53] =  1;
lookup_reserved_slot[54] =  1;
lookup_reserved_slot[55] =  1;
lookup_reserved_slot[56] =  1;
lookup_reserved_slot[57] =  1;
lookup_reserved_slot[58] =  1;
lookup_reserved_slot[59] =  1;

lookup_reserved_slot[60] =  1;
lookup_reserved_slot[61] =  1;
lookup_reserved_slot[62] =  1;
lookup_reserved_slot[63] =  1;
lookup_reserved_slot[64] =  1;
lookup_reserved_slot[65] =  1;
lookup_reserved_slot[66] =  1;
lookup_reserved_slot[67] =  1;
lookup_reserved_slot[68] =  1;
lookup_reserved_slot[69] =  1;
lookup_reserved_slot[70] =  1;
lookup_reserved_slot[71] =  1;
lookup_reserved_slot[72] =  1;
lookup_reserved_slot[73] =  1;
lookup_reserved_slot[74] =  1;
lookup_reserved_slot[75] =  1;
lookup_reserved_slot[76] =  1;
lookup_reserved_slot[77] =  1;
lookup_reserved_slot[78] =  1;
lookup_reserved_slot[79] =  1;
lookup_reserved_slot[80] =  1;
lookup_reserved_slot[81] =  1;
lookup_reserved_slot[82] =  1;
lookup_reserved_slot[83] =  1;
lookup_reserved_slot[84] =  1;
lookup_reserved_slot[85] =  1;
lookup_reserved_slot[86] =  1;
lookup_reserved_slot[87] =  1;
lookup_reserved_slot[88] =  1;
lookup_reserved_slot[89] =  1;

lookup_reserved_slot[90] =  1;
lookup_reserved_slot[91] =  1;
lookup_reserved_slot[92] =  1;
lookup_reserved_slot[93] =  1;
lookup_reserved_slot[94] =  1;
lookup_reserved_slot[95] =  1;
lookup_reserved_slot[96] =  1;
lookup_reserved_slot[97] =  1;
lookup_reserved_slot[98] =  1;
lookup_reserved_slot[99] =  1;
lookup_reserved_slot[100] =  1;
lookup_reserved_slot[101] =  1;
lookup_reserved_slot[102] =  1;
lookup_reserved_slot[103] =  1;
lookup_reserved_slot[104] =  1;
lookup_reserved_slot[105] =  1;
lookup_reserved_slot[106] =  1;
lookup_reserved_slot[107] =  1;
lookup_reserved_slot[108] =  1;
lookup_reserved_slot[109] =  1;
lookup_reserved_slot[110] =  1;
lookup_reserved_slot[111] =  1;
lookup_reserved_slot[112] =  1;
lookup_reserved_slot[113] =  1;
lookup_reserved_slot[114] =  1;
lookup_reserved_slot[115] =  1;
lookup_reserved_slot[116] =  1;
lookup_reserved_slot[117] =  1;
lookup_reserved_slot[118] =  1;
lookup_reserved_slot[119] =  1;



 { 
				0,.. 1  //10000000
				1,


}

lookup_free_slot

  
