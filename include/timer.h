#ifndef TIMER_H                
#define TIMER_H

#include "data_types/primitive_types.h"

typedef struct s_timer
{
	u32 val;
	void (*handler)(void*);
	void* handler_arg;
	void* ref;
}
t_timer;

t_timer* timer_init(int,void (*handler)(void* handler_arg),void* handler_arg,void* ref);
void timer_free(t_timer* timer);

#endif
