#include "system.h" 
#include "synchro_types/semaphore.h"

extern t_system system;

static int race1=0;
static int race2=0;

void sem_init(t_sem_desc* sem_desc)
{
	sem_desc->wait_queue=new_dllist();
	sem_desc->count=1;
	SPINLOCK_INIT(sem_desc->spinlock);		
}

void sem_down(t_sem_desc* sem_desc)
{
	struct t_process_context* current_process_context;
	unsigned int need_sleep=0;

	current_process_context=system.process_info.current_process->val;
	SPINLOCK_LOCK(sem_desc->spinlock);
	race1++;
	TRACE(30,PROC_PID)
	if (sem_desc->count==0)
	{
		current_process_context=system.process_info.current_process->val;
		ll_append( sem_desc->wait_queue,current_process_context);
		need_sleep=1;
		TRACE(31,PROC_PID)
	}
	else
	{
		sem_desc->count--;
	}
	TRACE(32,PROC_PID)
	race1--;
	if (race1>0)
	{
		panic();
	}
	SPINLOCK_UNLOCK(sem_desc->spinlock);
	
	if (need_sleep)
	{
		_sleep();
	}
}

void sem_up(t_sem_desc* sem_desc)
{
	t_llist_node* node;
	struct t_process_context* new_process_context;

	SPINLOCK_LOCK(sem_desc->spinlock);
	race2++;
	TRACE(33,PROC_PID)
	if (!ll_empty(sem_desc->wait_queue))
	{
		node=ll_first(sem_desc->wait_queue);
		new_process_context=(struct t_process_context*) node->val;
		ll_delete_node(node);
		_awake(new_process_context);
		TRACE(34,new_process_context->pid)
	}
	else
	{
		sem_desc->count++;
	}
	TRACE(35,PROC_PID)
	race2--;
	if (race2>0)
	{
		panic();
	}
	SPINLOCK_UNLOCK(sem_desc->spinlock);
}
