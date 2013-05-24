#ifndef ATA_H                
#define ATA_H

#include "scheduler/process.h"

#define READ_28 0x20
#define WRITE_28 0x30
#define REQUEST_WAITING 0x0;
#define REQUEST_COMPLETED 0x1;

typedef struct s_ata_desc
{
	unsigned int status;
	t_llist* pending_request;
	struct t_process_context *serving_process_context;
	
}
t_ata_desc;

void init_ata(t_ata_desc* ata_desc);
unsigned int _read_28_ata(unsigned int sector_count,unsigned int lba,void* io_buffer,unsigned int sync);
unsigned int _write_28_ata(unsigned int sector_count,unsigned int lba,void* io_buffer,unsigned int sync);
void free_ata();

#endif
