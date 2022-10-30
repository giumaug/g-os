#ifndef DEVICE_H                
#define DEVICE_H

#include "system.h"

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
	void* device_desc;
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
	u32 (*write_dma)(t_io_request* io_request);
	void dev;
	unsigned int status;
	t_io_request* serving_request;
	t_sem_desc mutex;
    t_sem_desc sem;
	u32 dma_pci_io_base;
	u32 dma_pci_mem_base;
	u8 dma_pci_bar_type;
	u8 num;
}
t_device_desc;

t_device_desc* init_device(u8 device_num);
void free_device(t_device_desc* device_desc);
u8 read_config_byte(t_device_desc* device_desc,u32 address);
u32 read_config_dword(t_device_desc* device_desc,u32 address);
void write_config_dword(t_device_desc* device_desc,u8 address,u32 value);
void write_config_byte(t_device_desc* device_desc,u32 address,u8 value);

#endif
