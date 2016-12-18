#include "timer.h"

t_timer* timer_init(val,handler,ref)
{
	t_timer* timer = NULL;

	timer = kamalloc(sizeof(t_timer));
	timer->val = val;
	timer->handler = handler;
	timer->ref = ref;
}

void timer_free(t_timer* timer)
{
	kfree(timer);
}
