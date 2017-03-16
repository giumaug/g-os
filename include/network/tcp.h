#ifndef TCP_H                
#define TCP_H

#include "system.h"
#include "timer.h"
#include "data_types/bit_vector.h"
#include "synchro_types/spin_lock.h"
#include "network/common.h"
#include "network/socket_buffer.h"

#define SMSS 			1454			
#define TCP_RCV_SIZE 		16384
#define TCP_SND_SIZE 		4096
//SPECS SAYS WND_ADV SHOULD BE TCP_RCV_SIZE. I USE TCP_RCV_SIZE FOR TEST
#define WND_ADV         	2048
#define TCP_CONN_MAP_SIZE 	20

//#define INC_WND(cur,wnd_size,offset)  (cur + offset) % wnd_size
#define INC_WND(cur,wnd_size,offset) (((cur + offset) <= wnd_size) ? (cur = cur + offset) : (cur = ((cur + offset) % wnd_size)))
#define SLOT_WND(cur,wnd_size) (cur % wnd_size)
#define DATA_IN_WND(min,max,index)										\
(																		\
	(min <= max) ?														\
	(																	\
		(index >= min && index <= max) ? 1 : 0							\
	)																	\
	:																	\
	(																	\
		(index >= max && index <= min) ? 1 : 0							\
	)																	\
)																		\

#define DATA_LF_OUT_WND(min,max,index) ((index < min) ? 1 : 0)
#define DATA_RH_OUT_WND(min,max,index) ((index > min) ? 1 : 0)

#define FLG_CWR 0b10000000
#define FLG_ECE 0b01000000
#define FLG_URG 0b00100000
#define FLG_ACK 0b00010000
#define FLG_PSH 0b00001000
#define FLG_RST 0b00000100
#define FLG_SYN 0b00000010
#define FLG_FIN 0b00000001

#define SYN_SENT		0
#define SYN_RCVD		1
#define ESTABILISHED 		2
#define FIN_WAIT_1		3
#define CLOSE_WAIT		4
#define FIN_WAIT_2		5
#define LAST_ACK		6	
#define TIME_WAIT		7
#define CLOSED                  8

//timout=200 ms (quantum = 10 ms)
#define PIGGYBACKING_TIMEOUT 20 
//3 sec should be 300
#define DEFAULT_RTO 500
#define SRTT_FACTOR 0.8

send_packet_tcp(tcp_conn_desc,NULL,0,ack_num,flags);
send_packet_tcp(tcp_conn_desc,NULL,0,ack_num,flags);

typedef struct s_tcp_snd_queue
{
	char* buf;
	u32 buf_size;
	u32 cur;
	u32 wnd_min;
	u32 wnd_size;
	u32 nxt_snd;
	//t_spinlock_desc lock;
}
t_tcp_snd_queue;

typedef struct s_tcp_rcv_queue
{
	char* buf;
	u32 buf_size;
	u32 wnd_min;
	u32 wnd_size;
	u32 nxt_rcv; //ack relativo finestra di ricezione e usato in trasmissione
	t_bit_vector* buf_state;
	//t_spinlock_desc lock;
}
t_tcp_rcv_queue;

typedef struct s_tcp_conn_desc
{
	u32 duplicated_ack;
	u32 rto;
	t_timer* rtrsn_timer;
	t_timer* pgybg_timer;
	u32 cwnd;
	u32 ssthresh;
	u32 rcv_wmd_adv;
	u32 max_adv_wnd;
	t_tcp_rcv_queue* rcv_queue;
	t_tcp_snd_queue* snd_queue;
	u32 seq_num;
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8 status;
	//WE NEED TO ADD A TIMEOUT SERVICE TO REMOVE INCOMPLETED REQUEST
	//AFTER 75 SECONDS
	struct s_tcp_conn_map* back_log_i_map;
	t_queue* back_log_c_queue;
	t_queue* data_wait_queue;
	struct t_process_context* process_context;
	u32 ref_count;
	t_spinlock_desc lock;
	u32 last_sent_time;
	u32 flight_size;
	u32 last_ack_sent;
	u32 syn_num;
	u32 fin_num;
}
t_tcp_conn_desc;

struct s_tcp_conn_map;

typedef struct s_tcp_desc
{
	struct s_tcp_conn_map* conn_map;		//ESTABILISHED CONNECTIONS MAP
	struct s_tcp_conn_map* req_map;			//REQUESTED CONNECTIONS MAP
	struct s_tcp_conn_map* listen_map;		//LISTENING CONNECTIONS MAP
	u32 listen_port_index;
}
t_tcp_desc;

struct s_data_sckt_buf;

t_tcp_conn_desc* tcp_conn_desc_int();
void tcp_conn_desc_free(t_tcp_conn_desc* tcp_conn_desc);
t_tcp_desc* tcp_init();
void tcp_free(t_tcp_desc* tcp_desc);
void rcv_packet_tcp(struct s_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len);
void update_snd_window(t_tcp_conn_desc* tcp_conn_desc,u32 ack_seq_num,u32 ack_data_len);
void rtrsn_timer_handler(void* arg);
void pgybg_timer_handler(void* arg);
int send_packet_tcp(t_tcp_conn_desc* tcp_conn_desc,char* data,u32 data_len,u32 ack_num,u8 flags);

#endif
