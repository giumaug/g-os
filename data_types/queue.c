#include "data_types/queue.h"

t_queue* new_queue()
{
	return new_dllist();
}

void enqueue(t_queue* queue,void* data)
{
	ll_prepend(queue,data);
}

void free_queue(t_queue* queue)
{
	ll_empty(queue);
}

void* dequeue(t_queue* queue)
{
	void* data=0;

	if (!ll_empty(queue))
        {
		t_llist_node* node=ll_last(queue);
		data=node->val;
		ll_delete_node(node);	
	}
	return data;
}
