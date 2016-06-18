#include "data_types/queue.h"

t_queue* new_queue()
{
	dc_new_queue(NULL);
}

t_queue* dc_new_queue(void (*data_destructor)(void*))
{
	t_queue* queue;

	queue=kmalloc(sizeof(t_queue));
	queue->data=new_dllist();
	queue->data_destructor=data_destructor;!!!!!!!!!!!!!!!!!!!qui
	return new_dllist();
}

void enqueue(t_queue* queue,void* datum)
{
	ll_prepend(queue,datum);
}

//USE ONLY AFTER DEQUEUE,QUEUE MUST BE EMPTY!!!!
void free_queue(t_queue* queue)
{
	free_llist(queue);
}

void* dequeue(t_queue* queue)
{
	void* datum=0;

	if (!ll_empty(queue))
    	{
		t_llist_node* node=ll_last(queue);
		datum=node->val;
		ll_delete_node(node);	
	}
	return datum;
}
