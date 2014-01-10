#ifndef SYSTEM_H                
#define SYSTEM_H

#include "console/console.h"
#include "scheduler/process.h"
#include "scheduler/scheduler.h"
#include "memory_manager/buddy.h"
#include "data_types/dllist.h"
#include "synchro_types/semaphore.h"
#include "ext2/ext2.h"

//struct s_device_desc;

typedef struct s_io_request 
{
	unsigned int sector_count;
	unsigned int lba;
	struct s_device_desc* device_desc;
	void* io_buffer;
	unsigned int status;
	struct t_process_context* process_context;
	unsigned int command;
}
t_io_request;

typedef struct s_device_desc
{
	u32 (*read)(t_io_request* io_request);
	u32 (*write)(t_io_request* io_request);
	unsigned int status;
	t_llist* pending_request;
	t_io_request* serving_request;
	t_semaphore sem;
	
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
	unsigned int time;
	struct t_process_info process_info;
	t_console_desc* active_console_desc;
	t_buddy_desc buddy_desc;
	unsigned int *master_page_dir;
	//t_llist* sleep_wait_queue;
	t_scheduler_desc scheduler_desc;
	struct s_ext2* root_fs;
	t_device_desc* device_desc;
	t_race_tracker race_tracker;
}
t_system;

#endif
