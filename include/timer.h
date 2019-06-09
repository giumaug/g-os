#ifndef TIMER_H                
#define TIMER_H

#include "system.h"
#include "data_types/primitive_types.h"

typedef struct s_timer
{
	u32 val;
	void (*handler)(void*);
	void* handler_arg;
	void* ref;
	int id;
}
t_timer;

t_timer* timer_init(int val,void (*handler)(void*),void* handler_arg,void* ref);
void timer_free(t_timer* timer);
void timer_set(t_timer* timer,long val);
void timer_reset(t_timer* timer);
void rdtscl(unsigned long long *ll);

#endif
