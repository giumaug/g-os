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

void rdtscl(unsigned long long *ll)
//inline static __attribute__((always_inline)) void rdtscl(unsigned long long *ll)
{
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));                        
    *ll = ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );  
}
