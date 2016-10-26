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
	bit_vector* buf_state;
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
	t_tcp_conn_map* conn_map;
	t_hashtable* bind_map;
	t_tcp_conn_map* back_log_c_map;
	t_tcp_conn_map* back_log_i_map;
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

u8 lookup_reserved_slot[128];
lookup_reserved_slot[0] =  1;  		// 10000000
lookup_reserved_slot[1] =  1;  		// 10000001
lookup_reserved_slot[2] =  1;  		// 10000010
lookup_reserved_slot[3] =  1;  		// 10000011
lookup_reserved_slot[4] =  1;  		// 10000100
lookup_reserved_slot[5] =  1;  		// 10000101
lookup_reserved_slot[6] =  1;  		// 10000110
lookup_reserved_slot[7] =  1;  		// 10000111
lookup_reserved_slot[8] =  1;  		// 10001000
lookup_reserved_slot[9] =  1;  		// 10001001
lookup_reserved_slot[10] =  1;  	// 10001010
lookup_reserved_slot[11] =  1;  	// 10001011
lookup_reserved_slot[12] =  1;  	// 10001100
lookup_reserved_slot[13] =  1;  	// 10001101
lookup_reserved_slot[14] =  1;  	// 10001110
lookup_reserved_slot[15] =  1;  	// 10001111
lookup_reserved_slot[16] =  1;  	// 10010000
lookup_reserved_slot[17] =  1;  	// 10010001
lookup_reserved_slot[18] =  1;  	// 10010010
lookup_reserved_slot[19] =  1;  	// 10010011
lookup_reserved_slot[20] =  1;  	// 10010100
lookup_reserved_slot[21] =  1;  	// 10010101
lookup_reserved_slot[22] =  1;  	// 10010110
lookup_reserved_slot[23] =  1;  	// 10010111
lookup_reserved_slot[24] =  1;  	// 10011000
lookup_reserved_slot[25] =  1;  	// 10011001
lookup_reserved_slot[26] =  1;  	// 10011010
lookup_reserved_slot[27] =  1;  	// 10011011
lookup_reserved_slot[28] =  1;  	// 10011100
lookup_reserved_slot[29] =  1;  	// 10011101
lookup_reserved_slot[30] =  1;  	// 10011110
lookup_reserved_slot[31] =  1;  	// 10011111
lookup_reserved_slot[32] =  1;  	// 10100000
lookup_reserved_slot[33] =  1;  	// 10100001
lookup_reserved_slot[34] =  1;  	// 10100010
lookup_reserved_slot[35] =  1;  	// 10100011
lookup_reserved_slot[36] =  1;  	// 10100100
lookup_reserved_slot[37] =  1;  	// 10100101
lookup_reserved_slot[38] =  1;		// 10100110
lookup_reserved_slot[39] =  1;  	// 10100111
lookup_reserved_slot[40] =  1;  	// 10101000
lookup_reserved_slot[41] =  1;		// 10101001
lookup_reserved_slot[42] =  1;  	// 10101010
lookup_reserved_slot[43] =  1;  	// 10101011
lookup_reserved_slot[44] =  1;  	// 10101100
lookup_reserved_slot[45] =  1;		// 10101101
lookup_reserved_slot[46] =  1;  	// 10101110
lookup_reserved_slot[47] =  1;  	// 10101111
lookup_reserved_slot[48] =  1;  	// 10110000
lookup_reserved_slot[49] =  1;  	// 10110001
lookup_reserved_slot[50] =  1;  	// 10110010
lookup_reserved_slot[51] =  1;  	// 10110011
lookup_reserved_slot[52] =  1;  	// 10110100
lookup_reserved_slot[53] =  1;  	// 10110101
lookup_reserved_slot[54] =  1;  	// 10110110
lookup_reserved_slot[55] =  1;  	// 10110111
lookup_reserved_slot[56] =  1;  	// 10111000
lookup_reserved_slot[57] =  1;  	// 10111001
lookup_reserved_slot[58] =  1;		// 10111010
lookup_reserved_slot[59] =  1;  	// 10111011
lookup_reserved_slot[60] =  1;		// 10111100
lookup_reserved_slot[61] =  1;  	// 10111101
lookup_reserved_slot[62] =  1;  	// 10111110
lookup_reserved_slot[63] =  1;  	// 10111111
lookup_reserved_slot[64] =  2;  	// 11000000
lookup_reserved_slot[65] =  2;  	// 11000001
lookup_reserved_slot[66] =  2;  	// 11000010
lookup_reserved_slot[67] =  2;  	// 11000011
lookup_reserved_slot[68] =  2;  	// 11000100
lookup_reserved_slot[69] =  2;  	// 11000101
lookup_reserved_slot[70] =  2;  	// 11000110
lookup_reserved_slot[71] =  2;  	// 11000111
lookup_reserved_slot[72] =  2;  	// 11001000
lookup_reserved_slot[73] =  2;  	// 11001001
lookup_reserved_slot[74] =  2;  	// 11001010
lookup_reserved_slot[75] =  2;  	// 11001011
lookup_reserved_slot[76] =  2;  	// 11001100
lookup_reserved_slot[77] =  2;  	// 11001101
lookup_reserved_slot[78] =  2;  	// 11001110
lookup_reserved_slot[79] =  2;  	// 11001111
lookup_reserved_slot[80] =  2;  	// 11010000
lookup_reserved_slot[81] =  2;  	// 11010001
lookup_reserved_slot[82] =  2;  	// 11010010
lookup_reserved_slot[83] =  2;  	// 11010011
lookup_reserved_slot[84] =  2;  	// 11010100
lookup_reserved_slot[85] =  2;  	// 11010101
lookup_reserved_slot[86] =  2;  	// 11010110
lookup_reserved_slot[87] =  2;  	// 11010111
lookup_reserved_slot[88] =  2;  	// 11011000
lookup_reserved_slot[89] =  2;  	// 11011001
lookup_reserved_slot[90] =  2;  	// 11011010
lookup_reserved_slot[91] =  2;  	// 11011011
lookup_reserved_slot[92] =  2;  	// 11011100
lookup_reserved_slot[93] =  2;  	// 11011101
lookup_reserved_slot[94] =  2;  	// 11011110
lookup_reserved_slot[95] =  2;  	// 11011111
lookup_reserved_slot[96] =  3;  	// 11100000
lookup_reserved_slot[97] =  3;  	// 11100001
lookup_reserved_slot[98] =  3;  	// 11100010
lookup_reserved_slot[99] =  3;  	// 11100011
lookup_reserved_slot[100] =  3;  	// 11100100
lookup_reserved_slot[101] =  3;  	// 11100101
lookup_reserved_slot[102] =  3;		// 11100110
lookup_reserved_slot[103] =  3;  	// 11100111
lookup_reserved_slot[104] =  3;  	// 11101000
lookup_reserved_slot[105] =  3;		// 11101001
lookup_reserved_slot[106] =  3;  	// 11101010
lookup_reserved_slot[107] =  3;  	// 11101011
lookup_reserved_slot[108] =  3;  	// 11101100
lookup_reserved_slot[109] =  3;		// 11101101
lookup_reserved_slot[110] =  3;  	// 11101110
lookup_reserved_slot[111] =  3;  	// 11101111
lookup_reserved_slot[112] =  4;  	// 11110000
lookup_reserved_slot[113] =  4;  	// 11110001
lookup_reserved_slot[114] =  4;  	// 11110010
lookup_reserved_slot[115] =  4;  	// 11110011
lookup_reserved_slot[116] =  4;  	// 11110100
lookup_reserved_slot[117] =  4;  	// 11110101
lookup_reserved_slot[118] =  4;  	// 11110110
lookup_reserved_slot[119] =  4;  	// 11110111
lookup_reserved_slot[120] =  5;  	// 11111000
lookup_reserved_slot[121] =  5;  	// 11111001
lookup_reserved_slot[122] =  5;		// 11111010
lookup_reserved_slot[123] =  6;  	// 11111011
lookup_reserved_slot[124] =  6;		// 11111100
lookup_reserved_slot[125] =  7;  	// 11111101
lookup_reserved_slot[126] =  7;  	// 11111110
lookup_reserved_slot[127] =  8;  	// 11111111

u8 lookup_free_slot[9];
lookup_free_slot[0] = 0;
lookup_free_slot[1] = 127;
lookup_free_slot[2] = 63;
lookup_free_slot[3] = 31;
lookup_free_slot[4] = 15;
lookup_free_slot[5] = 7;
lookup_free_slot[6] = 3;
lookup_free_slot[7] = 1;
lookup_free_slot[8] = 0;



  
