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
	if (timer->ref != NULL)
	{
  		ll_delete_node(timer->ref);
		timer->ref = NULL;
	}
	kfree(timer);
}

void timer_set(t_timer* timer,long val)
{
	if (timer->ref == NULL)
	{
		timer->val = val;
		timer->ref = ll_append(system.timer_list,timer);
	}
	else
	{
		timer->val = val;
	}
}

void timer_reset(t_timer* timer)
{
	if (timer->ref != NULL)
	{
		ll_delete_node(timer->ref);
		timer->ref = NULL;
		timer->val = 0;
	}
}
