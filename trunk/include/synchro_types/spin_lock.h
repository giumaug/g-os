#ifndef SPIN_LOCK_H                
#define SPIN_LOCK_H

#define SPINLOCK_INIT 
#define SPINLOCK_LOCK
#define SPINLOCK_UNLOCK

//ONLY IN MULTIPROCESSORS SYSTEMS.
//TO BE IMPLEMENTED.
//
//#define SPINLOCK_INIT spinlock_init(spinlock_desc)
//#define SPINLOCK_LOCK spinlock_lock(spinlock_desc)
//#define SPINLOCK_UNLOCK spinlock_unlock(spinlock_desc) 

//typedef struct s_spinlock_desc
//{
//	unsigned int status;
//}	
//t_spinlock_desc;
//
// #define SPINLOCK_INIT(t_spinlock_desc spinlock_desc) void spinlock_init(t_spinlock_desc spinlock_desc) 
//
// #define SPINLOCK_LOCK() void spinlock_lock(t_spinlock_desc spinlock_desc) 
//
// #define SPINLOCK_LOCK() void spinlock_unlock(t_spinlock_desc spinlock_desc) 

//HINT USING LOCAL LABEL

#define SPINLOCK_LOCK(lock)     \
{				\
__label__ spin;			\
asm ("                          \
	spin:			\
     	mov $1,%%eax;		\
	xchg %%eax,%0;		\
	cmp $0,%%eax;		\
	jne _spin;		\
     "				\
     ::"r"(lock):"%eax");       \
}			

#define SPINLOCK_UNLOCK(lock) 	\
asm("				\
	mov $0,%0		\
    "				\
    :"=r"(lock));

#define SPINLOCK_INIT(lock)     SPIN_UNLOCK(lock)



typedef struct s_spinlock_desc
{
	unsigned int status;
}	
t_spinlock_desc;






#endif

