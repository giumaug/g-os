#ifndef TCP_H                
#define TCP_H

#include "system.h"
#include "timer.h"
#include "data_types/bit_vector.h"
#include "synchro_types/spin_lock.h"
#include "network/common.h"
#include "network/socket_buffer.h"

#define SMSS 			1454	
#define TCP_RCV_SIZE 		(32768) 
#define TCP_USR_SIZE            (16384)
#define TCP_SND_SIZE 		(16384*256)
//#define TCP_SND_SIZE 		16384
//SPECS SAYS WND_ADV SHOULD BE TCP_RCV_SIZE. I USE TCP_RCV_SIZE FOR TEST
#define WND_ADV         	2048
#define TCP_CONN_MAP_SIZE 	20
#define EPHEMERAL_PORT_MAP_SIZE 20
#define CHK_OVRFLW(val,ref) ((unsigned int)val >= (unsigned int)ref ? (unsigned int)val : (long long)((long long)val + (long long)4294967295))
//#define CHK_OVRFLW(val,ref) val
#define INC_WND(cur,wnd_size,offset) (((cur + offset) <= wnd_size) ? (cur = cur + offset) : (cur = ((cur + offset) % wnd_size)))
#define SLOT_WND(cur,wnd_size) (cur % wnd_size)
#define DATA_IN_WND(min,max,index)								        \
(												                        \
	(min <= max) ?  (												    \
		(index >= min && index <= max) ? 1 : 0							\
	)												                    \
	:											                        \
	(												                    \
		(index >= max && index <= min) ? 1 : 0							\
	)												                    \
)													                    \

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
#define FIN_WAIT_1_PENDING      9
#define LAST_ACK_PENDING       10
#define RESET                  11

//timout=200 ms (quantum = 10 ms)
//#define PIGGYBACKING_TIMEOUT 4
#define PIGGYBACKING_TIMEOUT 4
//3 sec should be 300
#define SRTT_FACTOR 0.8
#define RTO_UBOUND 6000
#define RTO_LBOUND 100
#define RTO_VARIANCE 1.5
#define DEFAULT_RTO 100

#define _SEND_PACKET_TCP(tcp_conn_desc,data,data_len,ack_num,flags,seq_num) \
        send_packet_tcp(tcp_conn_desc->src_ip,                              \
                        tcp_conn_desc->dst_ip,                              \
                        tcp_conn_desc->src_port,                            \
                        tcp_conn_desc->dst_port,                            \
			            tcp_conn_desc->rcv_queue->wnd_size,                 \
                        data,                                               \
                        data_len,                                           \
                        ack_num,                                            \
                        flags, seq_num);                                    \
						/*equeue_packet_2(system.network_desc);*/

typedef struct s_tcp_log_item
{
	int action;
	int b_data_len;
	int a_data_len;
	int b_wnd_size;
    int a_wnd_size;
	int is_wnd_hole;
	int sleep;
	int pid;
	unsigned int b_wnd_min;
	unsigned int b_nxt_rcv;
	unsigned int a_wnd_min;
	unsigned int a_nxt_rcv;
	unsigned int b_max_seq;
	unsigned int a_max_seq;
	unsigned int b_seq;
	unsigned int a_seq;
}
t_tcp_log_item;

						
typedef struct s_tcp_profiler
{
	t_tcp_log_item item[250];
	int index;
}
t_tcp_profiler;

typedef struct s_tcp_snd_queue
{
	char* buf;
	u32 buf_size;
	u32 cur;
	u32 wnd_min;
	u32 wnd_size;
	u32 nxt_snd;
	u32 pnd_data;
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
	char* buf_state;
	u32 seq_num;
	u32 data_len;
	u32 last_adv_wnd;
	u32 buf_size_in_order;
	u8 is_wnd_hole;
	u32 max_seq_num;
	//u32 next_to_read;
}
t_tcp_rcv_queue;

typedef struct s_tcp_conn_desc
{
	u32 duplicated_ack;
	u32 rto;
	u32 srtt;
	struct s_timer* rtrsn_timer;
	struct s_timer* pgybg_timer;
	u32 cwnd;
	u32 ssthresh;
	u32 rcv_wmd_adv;
	u32 max_adv_wnd;
	t_tcp_rcv_queue* rcv_queue;
	t_tcp_snd_queue* snd_queue;
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8 status;
	//WE NEED TO ADD A TIMEOUT SERVICE TO REMOVE INCOMPLETED REQUEST
	//AFTER 75 SECONDS
	struct s_tcp_conn_map* back_log_i_map;
	t_queue* back_log_c_queue;
	struct t_process_context* process_context;
	u32 ref_count;
	t_spinlock_desc lock;
	u32 last_sent_time;
	u32 flight_size;
	u8 pending_ack;
	u8 isActive;
	u32 last_ack;
	//t_tcp_profiler log;
}
t_tcp_conn_desc;

struct s_tcp_conn_map;
//For evert s_tcp_conn_map struct src is this machine dst remote endpoint
typedef struct s_tcp_desc
{
	struct s_tcp_conn_map* conn_map;		//ESTABILISHED CONNECTIONS MAP
	struct s_tcp_conn_map* req_map;			//REQUESTED CONNECTIONS MAP
	struct s_tcp_conn_map* listen_map;		//LISTENING CONNECTIONS MAP
	u32 ep_port_index;
	struct s_hashtable* ep_port_map;
}
t_tcp_desc;

struct s_data_sckt_buf;

t_tcp_conn_desc* tcp_conn_desc_int();
void tcp_conn_desc_free(t_tcp_conn_desc* tcp_conn_desc);
t_tcp_desc* tcp_init();
void tcp_free(t_tcp_desc* tcp_desc);
void rcv_packet_tcp(struct s_data_sckt_buf* data_sckt_buf,u32 src_ip,u32 dst_ip,u16 data_len);
//void update_snd_window(t_tcp_conn_desc* tcp_conn_desc,u32 ack_seq_num,u32 ack_data_len);
void update_snd_window(t_tcp_conn_desc* tcp_conn_desc,u32 ack_seq_num,u32 ack_data_len,u8 skip_ack);
void rtrsn_timer_handler(void* arg);
void pgybg_timer_handler(void* arg);
int send_packet_tcp(u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port,u32 wnd_size,char* data,u32 data_len,u32 ack_num,u8 flags,u32 seq_num);
void update_adv_wnd(t_tcp_conn_desc* tcp_conn_desc);
//long long _chk_ovrflw(unsigned int val,unsigned int ref);

#endif
