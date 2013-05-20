#ifndef ATA_H                
#define ATA_H

#include "scheduler/process.h"

#define READ_28 0x20
#define WRITE_28 0x30
#define REQUEST_WAITING 0x0;
#define REQUEST_COMPLETED 0x1;

typedef struct s_ata_request
{
	unsigned int status;
	struct t_processor_reg* processor_reg;
	struct t_process_context *current_process_context;
}
t_ata_request;


typedef struct s_ata_desc
{
	unsigned int status;
	t_ata_request* current_ata_request;
	t_llist* pending_request;
	
}
t_ata_desc;

void init_ata(t_ata_desc* ata_desc);
unsigned int _read_28_ata(unsigned int sector_count,
			  unsigned int lba,
			  void* io_buffer,
			  struct t_processor_reg* processor_reg,
			  struct t_process_context *current_process_context
			  unsigned int sync);

unsigned int _write_28_ata(unsigned int sector_count,
			   unsigned int lba,void* io_buffer,
			   struct t_processor_reg* processor_reg,
			   struct t_process_context *current_process_context,
			   unsigned int sync);
void free_ata();

#endif
