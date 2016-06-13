#include "data_types/queue.h"

#define NULL 0

t_queue* new_queue()
{
	dc_new_queue(NULL);
}

t_queue* dc_new_queue(void (*data_destructor)(void*))
{
	t_queue* queue;

	queue=kmalloc(sizeof(t_queue));
	if (queue->data_destructor!=NULL) 
	{
		queue->data=dc_new_dllist(data_destructor);
	}
	else
	{
		queue->data=dc_new_dllist(NULL);
	}
	
	queue->data_destructor=data_destructor;
	return queue;
}

void enqueue(t_queue* queue,void* datum)
{
	ll_prepend(queue->data,datum);
}

void free_queue(t_queue* queue)
{
	free_llist(queue->data);
	kfree(queue);
}

void* dequeue(t_queue* queue)
{
	void* datum=0;

	if (!ll_empty(queue->data))
    	{
		t_llist_node* node=ll_last(queue->data);
		datum=node->val;
		ll_delete_node(node);	
	}
	return datum;
}
