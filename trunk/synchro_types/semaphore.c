#include "system.h" 
#include "synchro_types/semaphore.h"

extern t_system system;

void sem_init(t_sem_desc* sem_desc)
{
	sem_desc->wait_queue=new_dllist();
	sem_desc->count=1;
	SPINLOCK_INIT(sem_desc->spinlock);		
}

void sem_down(t_sem_desc* sem_desc)
{
	struct t_process_context* current_process_context;

	current_process_context=system.process_info.current_process->val;
	SPINLOCK_LOCK(sem_desc->spinlock);
	if (sem_desc->count==0)
	{
		current_process_context=system.process_info.current_process->val;
		ll_append( sem_desc->wait_queue,current_process_context);
		_sleep();
	}
	else
	{
		sem_desc->count--;
	}
	SPINLOCK_UNLOCK(sem_desc->spinlock);
}

void sem_up(t_sem_desc* sem_desc)
{
	t_llist_node* node;
	struct t_process_context* new_process_context;

	SPINLOCK_LOCK(sem_desc->spinlock);
	if (!ll_empty(sem_desc->wait_queue))
	{
		node=ll_first(sem_desc->wait_queue);
		new_process_context=(struct t_process_context*) node->val;
		ll_delete_node(node);
		_awake(new_process_context);
	}
	else
	{
		sem_desc->count++;
	}
	SPINLOCK_UNLOCK(sem_desc->spinlock);
}
