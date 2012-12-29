#ifndef SYSTEM_H                
#define SYSTEM_H

#include "console/console.h"
#include "scheduler/process.h"

typedef struct s_system
{
	struct t_process_info process_info;
	t_console_desc* active_console_desc;
	unsigned int *master_page_dir;
	unsigned int* page_pad;
}
t_system;

#endif
