#ifndef SYSTEM_H                
#define SYSTEM_H

#include "data_types/dllist.h"
#include "data_types/primitive_types.h"
#include "data_types/hashtable.h"
#include "synchro_types/spin_lock.h"
#include "synchro_types/semaphore.h"
#include "scheduler/process.h"
#include "console/console.h"
#include "scheduler/scheduler.h"
#include "memory_manager/buddy.h"

#define DEBUG
#ifdef DEBUG 
	#define PRINTK(...) printk(__VA_ARGS__); 
#else 
	#define PRINTK(__VA_ARGS__) 
#endif

#define NULL 0
#define TRUE 1
#define FALSE 0

#define DEVICE_IDLE 0
#define DEVICE_BUSY 1
#define POOLING_MODE 2

struct s_ext2;
struct s_scheduler_desc;
struct s_buddy_desc;

typedef struct s_io_request 
{
	unsigned int sector_count;
	unsigned int lba;
	struct s_device_desc* device_desc;
	void* io_buffer;
	struct t_process_context* process_context;
	unsigned int command;
}
t_io_request;

typedef struct s_device_desc
{
	u32 (*read)(t_io_request* io_request);
	u32 (*write)(t_io_request* io_request);
	u32 (*p_read)(t_io_request* io_request);
	u32 (*p_write)(t_io_request* io_request);

	unsigned int status;
	t_io_request* serving_request;
	t_sem_desc mutex;
	t_sem_desc sem;
}
t_device_desc;

typedef struct s_system
{
	unsigned int time;
	struct t_process_info* process_info;
	t_console_desc* active_console_desc;
	struct s_buddy_desc* buddy_desc;
	unsigned int *master_page_dir;
	struct s_scheduler_desc* scheduler_desc;
	struct s_ext2* root_fs;
	t_device_desc* device_desc;
	unsigned int int_path_count;
//	unsigned int tracepoint_index;
//	t_tracepoint tracepoint[200000];
	unsigned int panic;
}
t_system;

extern t_system system;

#endif
