#ifndef TIMER_H                
#define TIMER_H

#include "data_types/primitive_types.h"

struct s_timer
{
	u32 val;
	void (*handler)(void*);
	void* handler_arg;
	void* ref;
}
typedef t_timer;

t_timer* timer_init(val,handler,ref);
void timer_free(t_timer* timer);

#endif
