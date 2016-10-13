#define SMSS 1454
#define TCP_RCV_SIZE 
#define TCP_SND_SIZE
#define TCP_CONN_MAP_SIZE 	20

#define INC_WND(cur,wnd_size,offset)  (cur + offset) % wnd_size
#define SLOT_WND (cur,wnd_size) (cur % wnd_size
#define DATA_IN_WND(min,max,index)									\
(													\
	(min <= max) ?											\
	(												\
		(index >= min && index <= max) ? 1 : 0							\
	)												\
	:												\
	(												\
		(index >= max && index <= min) ? 1 : 0							\
	)												\
)													\

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
	u32 rto;
	u32 rtrsn_timer;
	u32 pgybg_timer;
	t_tcp_rcv_queue* rcv_queue;
	t_tcp_snd_queue* snd_queue;
	u32 seq_num;
	u32 ack_seq_num;
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	t_socket* socket;
	//WE NEED TO ADD A TIMEOUT SERVICE TO REMOVE INCOMPLETED REQUEST
	//AFTER 75 SECONDS
	struct t_queue* back_log_i_queue;
	struct t_queue* back_log_c_queue;
}
t_tcp_conn_desc;

typedef struct s_tcp_desc
{
	t_tcp_conn_map* conn_map;			//ESTABILISHED CONNECTIONS
	t_tcp_conn_map* req_map;			//REQUESTED CONNECTIONS
	t_tcp_conn_map* listen_map;			//LISTENING CONNECTIONS
	u32 listen_port_index;
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
