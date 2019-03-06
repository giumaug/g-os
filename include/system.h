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
#include "network/network.h"
#include "timer.h"

#define TEST_STACK 0x101636
#define TEST_USER_SPACE 0xe8f87d81
#define AFTER_FORK 0x100179

//#define DEBUG
#ifdef DEBUG 
	#define PRINTK(...) printk(__VA_ARGS__); 
#else
	#define PRINTK(...)
#endif

#define DEVICE_IDLE 0
#define DEVICE_BUSY 1
#define POOLING_MODE 2

struct s_ext2;
struct s_scheduler_desc;
struct s_buddy_desc;
struct s_device_desc;
struct s_network_desc;

typedef struct s_dma_lba
{
	unsigned int lba;
	unsigned short sector_count;
}
t_dma_lba;

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
	u32 (*read_dma)(t_io_request* io_request);
	unsigned int status;
	t_io_request* serving_request;
	t_sem_desc mutex;
        t_sem_desc sem;
	u32 dma_pci_io_base;
	u32 dma_pci_mem_base;
	u8 dma_pci_bar_type;
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
	struct s_device_desc* device_desc;
	unsigned int int_path_count;
	struct s_network_desc* network_desc;
	unsigned int panic;
	t_llist* timer_list;
	u8 force_scheduling;
	u8 flush_network;
	u32 fork;
	u32 out;
	u32 piggy_timeout;
	u32 rtrsn_timeout;
	u32 reset;	
	u32 tcp_1;
	u32 tot_int;
	u32 packet_sent;
	u32 packet_received;
	u32 counter;
	u32 max_processed_packet;
	u32 sleep_count;
}
t_system;

extern t_system system;

#endif
