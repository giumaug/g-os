#ifndef ATA_H                
#define ATA_H

#include "system.h"
#include "scheduler/process.h"
#include "drivers/ata/ata.h"

#define READ_28 0x20
#define WRITE_28 0x30
#define REQUEST_WAITING 0x0
#define REQUEST_COMPLETED 0x1

void init_ata(t_device_desc* ata_desc);
unsigned int _read_28_ata(t_device_desc* ata_desc,unsigned int sector_count,unsigned int lba,void* io_buffer,unsigned int sync);
unsigned int _write_28_ata(t_device_desc* ata_desc,unsigned int sector_count,unsigned int lba,void* io_buffer,unsigned int sync);
void free_ata();

#endif
