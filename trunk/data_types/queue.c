#include "general.h"
#include "data_types/queue.h"

int dumpq[1000];

t_queue *new_queue()
{
	return new_dllist();
}

void enqueue(t_queue *queue,void *data)
{
	ll_prepend(queue,data);
}

void free_queue(t_queue *queue)
{
	ll_empty(queue);
}

void *dequeue(t_queue *queue)
{
	void *data=NULL;
	if (!ll_empty(queue))
        {
		t_llist_node *node=ll_last(queue);
		data=node->val;
		ll_delete_node(node);	
	}
	return data;
}

void dump_queue(t_queue *queue)
{
//	int xx=-1;
//	if (!ll_empty(queue))
//        {
//		t_llist_node *node=ll_last(queue);
//		xx++;
//		dumpq[xx]=node->val;	
//	}
}
