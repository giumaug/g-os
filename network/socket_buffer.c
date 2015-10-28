

typedef struct data_sckt_buf
{
	unsigned char* trasport_hdr;
	unsigned char* network_hdr;
	unsigned char* mac_hdr;
	unsigned char* tail;
	unsigned char* end;
	unsigned char* head;
	unsigned char* data;
}
t_data_sckt_buf;

typedef struct sckt_buf_desc
{
	t_queue* buf;
	u32 buf_size;
	u32 buf_index;
	t_spinlock_desc lock;
}
t_sckt_buf_desc;

static void sckt_buf_desc_init()
{
	sckt_buf_desc.buf=new_queue();
	sckt_buf_desc.buf.buf_size=SOCKET_BUFFER_SIZE;
	sckt_buf_desc.buf.buf_index=0;
	SPINLOCK_INIT(sckt_buf_desc.buf.lock);
}

enqueue_packet(t_sckt_buf_desc* sckt_buf_desc,t_data_sckt_buf* data_sckt_buf)
{					
	SPINLOCK_LOCK(sckt_buf_desc->lock);			
	if (sckt_buf_desc->buf_index+1<=sckt_buf_desc.buf_size)
	{							
		enqueue(sckt_buf_desc->buf,data_sckt_buf);	
		sckt_buf_desc->buf_index++;			
	}							
	SPINLOCK_UNLOCK(sckt_buf_desc->lock);
}
qui
#define DEQUEUE_PACKET(data_sckt_buf)					\
		SPINLOCK_LOCK(sckt_buf_desc.lock);			\
		if (ip4_desc.buf_index>0)				\
		{							\
			data_sckt_buf=denqueue(sckt_buf_desc.buf);	\
			sckt_buf_desc.buf_index--;			\
		}							\
		SPINLOCK_UNLOCK(sckt_buf_desc.lock);
