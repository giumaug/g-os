#ifndef SEMAPHORE_H                
#define SEMAPHORE_H

#include "data_types/dllist.h"
#include "scheduler/process.h"
#include "synchro_types/spin_lock.h"

typedef struct s_sem_desc 
{
	t_llist* wait_queue;
	unsigned int count;
	t_spinlock_desc spinlock;
}
t_sem_desc;

void sem_init(t_sem_desc* sem_desc);
void sem_down(t_sem_desc* sem_desc);
void sem_up(t_sem_desc* sem_desc);

#endif
