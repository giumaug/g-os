#ifndef SOCKET_BUFFER_H                
#define SOCKET_BUFFER_H

#include "system.h"
#include "data_types/queue.h"
#include "network/network.h"
#include "synchro_types/spin_lock.h"

typedef struct s_data_sckt_buf
{
	unsigned char* transport_hdr;
	unsigned char* network_hdr;
	unsigned char* mac_hdr;
	unsigned char* data;
	u16 data_len;
}
t_data_sckt_buf;

typedef struct s_sckt_buf_desc
{
	t_queue* buf;
	u32 buf_size;
	u32 buf_index;
	t_spinlock_desc lock;
}
t_sckt_buf_desc;

t_sckt_buf_desc* sckt_buf_desc_init();
void sckt_buf_desc_free(t_sckt_buf_desc* sckt_buf_desc);
void enqueue_sckt(t_sckt_buf_desc* sckt_buf_desc,t_data_sckt_buf* data_sckt_buf);
t_data_sckt_buf* dequeue_sckt(t_sckt_buf_desc* sckt_buf_desc);
t_data_sckt_buf* alloc_sckt(u16 data_len);
t_data_sckt_buf* alloc_void_sckt();
void free_sckt(t_data_sckt_buf* data_sckt_buf);
unsigned short checksum(unsigned short* ip, int len);

#endif
