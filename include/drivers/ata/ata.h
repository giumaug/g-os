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
	struct t_processor_reg* processor_reg;
	//qui......
	
}
t_ata_request;

void init_ata();
unsigned int _read_28_ata(t_ata_request *ata_request,unsigned int sync);
unsigned int _write_28_ata(t_ata_request *ata_request,unsigned int sync);
void free_ata();

#endif
