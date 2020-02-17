#include "system.h"
//#include "synchro_types/semaphore.h"
#include "asm.h"

//CESATI AND BOVE explain how to protect kernel data structures at pag 216 parag. 5.3.
//Intresting also is chapter 5 of Linux Device Drivers.
//With count zero init mutex (formally process holding mutex should be the only permitted to release mutex )
//Process holding mutex or semaphore can sleep.Process holding spinlock can't.(Performance problems or deadlock)
//Spinlock are useless in uniprocessor system without preemption.In uniprocessor system with preemption spinlock 
//could cause deadlock:if i use a spinlock and interrupt is fired when spinlock 
//is held system get deadlocked.
//In multiporcessor system spinlock needs to disable local interrupt.
//If the spinlook is used only in race between exception it is still possible to have
//interrupt enabled.Linux offer two variant of spinlock.

void sem_init(t_sem_desc* sem_desc,int count)
{
	sem_desc->wait_queue=new_dllist();
	sem_desc->count=count; //1
//	SPINLOCK_INIT(sem_desc->spinlock);		
}

void sem_down(t_sem_desc* sem_desc)
{
	struct t_process_context* current_process_context;
	unsigned int need_sleep=0;

	current_process_context=system.process_info->current_process->val;
	
//	SPINLOCK_LOCK(sem_desc->spinlock);
	SAVE_IF_STATUS	
	CLI
	
	if (sem_desc->count==0)
	{
		current_process_context=system.process_info->current_process->val;
		ll_append( sem_desc->wait_queue,current_process_context);
		need_sleep=1;
	}
	else
	{
		sem_desc->count--;
	}
	
//	SPINLOCK_UNLOCK(sem_desc->spinlock);
//	RESTORE_IF_STATUS
	
	if (need_sleep)
	{
		_sleep();
	}
	RESTORE_IF_STATUS
}

void sem_up(t_sem_desc* sem_desc)
{
	t_llist_node* node;
	struct t_process_context* new_process_context;
	struct t_process_context* current_process;

	CURRENT_PROCESS_CONTEXT(current_process);

//	SPINLOCK_LOCK(sem_desc->spinlock);
	SAVE_IF_STATUS	
	CLI

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

//	SPINLOCK_UNLOCK(sem_desc->spinlock);
	RESTORE_IF_STATUS
}
