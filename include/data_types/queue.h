#ifndef QUEUE_H                
#define QUEUE_H

#include "data_types/dllist.h"

struct s_queue
{
	t_llist* data;
	void (*data_destructor)(void*);
}
typedef t_queue;

t_queue* new_queue();
t_queue* dc_new_queue(void (*data_destructor)(void*));
void free_queue(t_queue* queue);
void enqueue(t_queue* queue,void* data);
void *dequeue(t_queue* queue);

#endif
