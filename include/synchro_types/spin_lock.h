#ifndef SPIN_LOCK_H                
#define SPIN_LOCK_H

#include "system.h"

//HINT:%= expands in unique integer


#define ENABLE_PREEMPTION asm("lock;decl %0;":"=m"(system.int_path_count):"m"(system.int_path_count));
#define DISABLE_PREEMPTION asm("lock;incl %0;":"=m"(system.int_path_count):"m"(system.int_path_count));


#define _SPINLOCK_LOCK(lock)    \
asm ("                          \
	_spin%=:		\
     	mov $0x1,%%eax;         \
	xchg %%eax,%0;		\
	cmp $0,%%eax;		\
	jne _spin%=;		\
     "                          \
    :"+r"((lock).status)::"%eax");
			

#define _SPINLOCK_UNLOCK(lock) 	\
asm("				\
	mov $0,%0		\
    "				\
    :"+r"((lock).status));

#define SPINLOCK_LOCK(lock)	\
	DISABLE_PREEMPTION	\
	_SPINLOCK_LOCK(lock)

#define SPINLOCK_UNLOCK(lock)	\
	_SPINLOCK_UNLOCK(lock)	\
	ENABLE_PREEMPTION		

#define SPINLOCK_INIT(lock)     _SPINLOCK_UNLOCK(lock)


typedef struct s_spinlock_desc
{
	unsigned int status;
}	
t_spinlock_desc;

#endif

