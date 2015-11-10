

typedef struct data_sckt_buf
{
	unsigned char* transport_hdr;
	unsigned char* network_hdr;
	unsigned char* mac_hdr;
	unsigned char* head		//start protocol memory area
	unsigned char* end;    		//end protocol memory area
	unsigned char* data;   		//start data memory area
	unsigned char* tail;   		//end data memory area
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

//FROM NOW EVERY INIT MUST FOLLOW THIS PATTERN!!!!!!!!!!!!!!!!!!!!!!!!!!
t_sckt_buf_desc* sckt_buf_desc_init(t_sckt_buf_desc* sckt_buf_desc)
{
	t_sckt_buf_desc* sckt_buf_desc=kmalloc(sizeof(t_sckt_buf_desc));
	sckt_buf_desc->buf=new_queue();
	sckt_buf_desc->buf_size=SOCKET_BUFFER_SIZE;
	sckt_buf_desc->buf_index=0;
	SPINLOCK_INIT(sckt_buf_desc.buf->lock);
	return sckt_buf_desc;
}

void sckt_buf_desc_free(t_sckt_buf_desc* sckt_buf_desc)
{
	free_queue(sckt_buf_desc->buf);	
}

void enqueue_sckt(t_sckt_buf_desc* sckt_buf_desc,t_data_sckt_buf* data_sckt_buf)
{					
	SPINLOCK_LOCK(sckt_buf_desc->lock);			
	if (sckt_buf_desc->buf_index+1<=sckt_buf_desc.buf_size)
	{							
		enqueue(sckt_buf_desc->buf,data_sckt_buf);	
		sckt_buf_desc->buf_index++;			
	}							
	SPINLOCK_UNLOCK(sckt_buf_desc->lock);
}

t_data_sckt_buf* dequeue_sckt(t_sckt_buf_desc* sckt_buf_desc)
{
		SPINLOCK_LOCK(sckt_buf_desc->lock);
		if (sckt_buf_desc->buf_index>0)
		{
			data_sckt_buf=denqueue(sckt_buf_desc->buf);
			sckt_buf_desc->buf_index--;
		}
		SPINLOCK_UNLOCK(sckt_buf_desc->lock);
		return data_sckt_buf;
}

t_data_sckt_buf* alloc_sckt(u16 data_len)
{
	t_data_sckt_buf* data_sckt_buf=kmalloc(sizeof(t_data_sckt_buf));
	data_sckt_buf->head=kmalloc(data_len);
	data_sckt_buf->end=data_sckt_buf->head+data_len;
	return data_sckt_buf;
}

void free_sckt(t_data_sckt_buf* data_sckt_buf)
{
	kfree(data_sckt_buf->head);
}



