#ifndef QUEUE_H                
#define QUEUE_H

#include "data_types/dllist.h"

typedef t_llist t_queue;

t_queue* new_queue();
void free_queue(t_queue* queue);
void enqueue(t_queue* queue,void* data);
void *dequeue(t_queue* queue);

#endif
