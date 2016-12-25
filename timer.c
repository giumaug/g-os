#include "timer.h"

t_timer* timer_init(int,(*handler)(void* handler),void* ref)
{
	t_timer* timer = NULL;

	timer = kamalloc(sizeof(t_timer));
	timer->val = val;
	timer->handler = handler;
	timer->ref = ref;????+30
}

void timer_free(t_timer* timer)
{
	kfree(timer);
}
