#ifndef SYSTEM_H                
#define SYSTEM_H

#include "console/console.h"
#include "scheduler/process.h"
#include "scheduler/scheduler.h"
#include "memory_manager/buddy.h"
#include "data_types/dllist.h"
#include "ext2/ext2.h"

typedef struct s_device_desc
{
	u32 (*read)(void* device_desc,unsigned int sector_count,unsigned int lba,void* io_buffer,unsigned int sync);
	u32 (*write)(void* device_desc,unsigned int sector_count,unsigned int lba,void* io_buffer,unsigned int sync);
	unsigned int status;
	t_llist* pending_request;
	struct t_process_context* serving_process_context;
	
}
t_device_desc;

typedef struct s_system
{
	struct t_process_info process_info;
	t_console_desc* active_console_desc;
	t_buddy_desc buddy_desc;
	unsigned int *master_page_dir;
	t_llist* sleep_wait_queue;
	t_scheduler_desc scheduler_desc;
	t_ext2* root_fs;
}
t_system;

#endif
