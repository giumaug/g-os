#include "network/socket_buffer.h"
#include "debug.h"

//FROM NOW EVERY INIT MUST FOLLOW THIS PATTERN!!!!!!!!!!!!!!!!!!!!!!!!!!
t_sckt_buf_desc* sckt_buf_desc_init()
{
	t_sckt_buf_desc* sckt_buf_desc=kmalloc(sizeof(t_sckt_buf_desc));
	//collect_mem_alloc(sckt_buf_desc);
	sckt_buf_desc->buf=new_queue();
	sckt_buf_desc->buf_size=SOCKET_BUFFER_SIZE;
	sckt_buf_desc->buf_index=0;
	SPINLOCK_INIT(sckt_buf_desc->lock);
	return sckt_buf_desc;
}

void sckt_buf_desc_free(t_sckt_buf_desc* sckt_buf_desc)
{
	free_queue(sckt_buf_desc->buf);	
}

void enqueue_sckt(t_sckt_buf_desc* sckt_buf_desc,t_data_sckt_buf* data_sckt_buf)
{	
	//printk("in enqueue \n");				
	SPINLOCK_LOCK(sckt_buf_desc->lock);			
	if (sckt_buf_desc->buf_index+1<=sckt_buf_desc->buf_size)
	{	
		//printk("--qne %d \n",data_sckt_buf);						
		enqueue(sckt_buf_desc->buf,data_sckt_buf);	
		sckt_buf_desc->buf_index++;			
	}							
	SPINLOCK_UNLOCK(sckt_buf_desc->lock);
	//printk("exit enqueue \n");
}

t_data_sckt_buf* dequeue_sckt(t_sckt_buf_desc* sckt_buf_desc)
{
		t_data_sckt_buf* data_sckt_buf=NULL;

		//printk("in denqueue \n");
		SPINLOCK_LOCK(sckt_buf_desc->lock);
		if (sckt_buf_desc->buf_index>0)
		{
			data_sckt_buf=dequeue(sckt_buf_desc->buf);
			sckt_buf_desc->buf_index--;
		}
		SPINLOCK_UNLOCK(sckt_buf_desc->lock);
		//printk("exit denqueue \n");
		return data_sckt_buf;
}

t_data_sckt_buf* alloc_sckt(u16 data_len)
{
	char* data;
	t_data_sckt_buf* data_sckt_buf=kmalloc(sizeof(t_data_sckt_buf));
	collect_mem_alloc(data_sckt_buf);
	printk("allocating %d \n",data_sckt_buf);
	data=kmalloc(data_len);
	collect_mem_alloc(data);
	printk("allocating d %d \n",data);
	data_sckt_buf->data=data;
	data_sckt_buf->data_len=data_len;
	return data_sckt_buf;
}

t_data_sckt_buf* alloc_void_sckt()
{
	t_data_sckt_buf* data_sckt_buf=kmalloc(sizeof(t_data_sckt_buf));
	collect_mem_alloc(data_sckt_buf);
	printk("allocating v %d \n",data_sckt_buf);
	data_sckt_buf->data=NULL;
	data_sckt_buf->data_len=0;
	return data_sckt_buf;
}

void free_sckt(t_data_sckt_buf* data_sckt_buf)
{
	if (data_sckt_buf->data!=NULL)
	{
		kfree(data_sckt_buf->data);
		printk("free d %d \n",data_sckt_buf->data);
		collect_mem_free(data_sckt_buf->data);
	}
	kfree(data_sckt_buf);
	printk("free %d \n",data_sckt_buf);
	collect_mem_free(data_sckt_buf);
}

unsigned short checksum(unsigned short* ip, int len)
{
	long sum = 0;  /* assume 32 bit long, 16 bit short */

	while(len > 1)
	{
		//sum += *((unsigned short*) ip)++; orig line
             	sum += *(ip++);
             	if(sum & 0x80000000)
		{
			/* if high order bit set, fold */
               		sum = (sum & 0xFFFF) + (sum >> 16);
		}		
             	len -= 2;
	}

        if(len)
	{         
		/* take care of left over byte */
             	sum += (*ip & 0xFF);
	}
          
        unsigned int ddd=sum;
	while(sum>>16)
	{
             sum = (sum & 0xFFFF) + (sum >> 16);
	}
	//need to swap because x86 is little endian
	return ~(unsigned short)sum;
}
