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
	u32 (*read)(void* device_desc,unsigned int sector_count,unsigned int lba,void* io_buffer);
	u32 (*write)(void* device_desc,unsigned int sector_count,unsigned int lba,void* io_buffer);
	unsigned int status;
	t_llist* pending_request;
	struct t_process_context* serving_request;
	
}
t_device_desc;

struct s_ext2;

typedef struct s_race_tracker
{
	unsigned char buffer[256];
	unsigned char index;
	char* stack;
	int f[1000];
	int v[1000];
	int mem_index;
}
t_race_tracker;

typedef struct s_system
{
	struct t_process_info process_info;
	t_console_desc* active_console_desc;
	t_buddy_desc buddy_desc;
	unsigned int *master_page_dir;
	t_llist* sleep_wait_queue;
	t_scheduler_desc scheduler_desc;
	struct s_ext2* root_fs;
	t_device_desc* device_desc;
	t_race_tracker race_tracker;
}
t_system;

#endif
