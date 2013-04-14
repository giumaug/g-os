#ifndef ATA_H                
#define ATA_H

#include "scheduler/process.h"

#define READ_28 0x20
#define WRITE_28 0x30

typedef struct s_ata_request
{
	void *io_buffer;
	unsigned int lba;
	unsigned char sector_count;
	struct t_process_context *process_context;
	unsigned char cmd;
	 
}
t_ata_request;

void init_ata();
void _read_28_ata(t_ata_request *ata_request,struct t_processor_reg* processor_reg);
void _write_28_ata(t_ata_request *ata_request,struct t_processor_reg* processor_reg);
void free_ata();

#endif
