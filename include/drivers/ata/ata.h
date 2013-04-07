#ifndef ATA_H                
#define ATA_H

#include "scheduler/process.h"

typedef struct s_ata_request
{
	void *io_buffer;
	unsigned int lba;
	char sector_count;
	struct t_process_context *process_context; 
}
t_ata_request;

void init_ata();
void _read_28_ata(t_ata_request *ata_request);
void free_ata();
void write_28_ata();

#endif
