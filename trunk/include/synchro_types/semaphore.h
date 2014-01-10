#ifndef SEMAPHORE_H                
#define SEMAPHORE_H

#include "data_types/dllist.h"
#include "scheduler/process.h"
#include "synchro_types/spin_lock.h"

typedef struct s_sem_desc 
{
	t_llist* wait_queue wait_queue;
	unsigned int count;
	unsigned int spinlock;
}
t_sem_desc;

void sem_int(t_sem_desc sem_desc);
void sem_down(t_sem_desc sem_desc);
void sem_up(t_sem_desc sem_desc);

#endif
