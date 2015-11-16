#include "network/socket_buffer.h"

//FROM NOW EVERY INIT MUST FOLLOW THIS PATTERN!!!!!!!!!!!!!!!!!!!!!!!!!!
t_sckt_buf_desc* sckt_buf_desc_init()
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
	t_data_sckt_buf* data_sckt_buf=kmalloc(sizeof(t_data_sckt_buf)+data_len);
	data_sckt_buf->head=data_sckt_buf+sizeof(t_data_sckt_buf);
	data_sckt_buf->end=data_sckt_buf->head+data_len;
	return data_sckt_buf;
}

void free_sckt(t_data_sckt_buf* data_sckt_buf)
{
	kfree(data_sckt_buf);
}



