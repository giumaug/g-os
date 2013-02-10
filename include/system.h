#ifndef SYSTEM_H                
#define SYSTEM_H

#include "console/console.h"
#include "scheduler/process.h"
#include "memory_manager/buddy.h"

typedef struct s_system
{
	struct t_process_info process_info;
	t_console_desc* active_console_desc;
	t_buddy_desc buddy_desc;
	unsigned int *master_page_dir;
}
t_system;

#endif
