#ifndef SPIN_LOCK_H                
#define SPIN_LOCK_H

//HINT:%= expands in unique integer

#define SPINLOCK_LOCK(lock)     \
asm ("                          \
	_spin%=:		\
     	mov $0x1,%%eax;         \
	xchg %%eax,%0;		\
	cmp $0,%%eax;		\
	jne _spin%=;		\
     "                          \
    ::"r"(lock.status):"%eax");
			

#define SPINLOCK_UNLOCK(lock) 	\
asm("				\
	mov $0,%0		\
    "				\
    :"=r"(lock.status));

#define SPINLOCK_INIT(lock)     SPINLOCK_UNLOCK(lock)



typedef struct s_spinlock_desc
{
	unsigned int status;
}	
t_spinlock_desc;

#endif

