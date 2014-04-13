#include "system.h" 
#include "synchro_types/semaphore.h"

extern t_system system;

static int race1=0;
static int race2=0;

//With count zero init mutex (formally process holding mutex should be the only permitted to release mutex )
//Process holding mutex or semaphore can sleep.Process holding spinlock can't.
void sem_init(t_sem_desc* sem_desc,int count)
{
	sem_desc->wait_queue=new_dllist();
	sem_desc->count=count; //1
	SPINLOCK_INIT(sem_desc->spinlock);		
}

void sem_down(t_sem_desc* sem_desc)
{
	struct t_process_context* current_process_context;
	unsigned int need_sleep=0;

	current_process_context=system.process_info.current_process->val;
	SPINLOCK_LOCK(sem_desc->spinlock);
	race1++;
	if (sem_desc->count==0)
	{
		current_process_context=system.process_info.current_process->val;
		ll_append( sem_desc->wait_queue,current_process_context);
		need_sleep=1;
	}
	else
	{
		sem_desc->count--;
	}
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
	race2--;
	if (race2>0)
	{
		panic();
	}
	SPINLOCK_UNLOCK(sem_desc->spinlock);
}
