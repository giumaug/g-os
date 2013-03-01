#ifndef SYSTEM_H                
#define SYSTEM_H

#include "console/console.h"
#include "scheduler/process.h"
#include "scheduler/scheduler.h"
#include "memory_manager/buddy.h"
#include "data_types/dllist.h"

typedef struct s_system
{
	struct t_process_info process_info;
	t_console_desc* active_console_desc;
	t_buddy_desc buddy_desc;
	unsigned int *master_page_dir;
	t_llist* sleep_wait_queue;
	t_scheduler_desc scheduler_desc;
}
t_system;

#endif
