#include "timer.h"

t_timer* timer_init(int val,void (*handler)(void*),void* handler_arg,void* ref)
{
	t_timer* timer = NULL;

	timer = kmalloc(sizeof(t_timer));
	timer->val = val;
	timer->handler = handler;
	timer->handler_arg = handler_arg;
	timer->ref = ref;
}

void timer_free(t_timer* timer)
{
	kfree(timer);
}
