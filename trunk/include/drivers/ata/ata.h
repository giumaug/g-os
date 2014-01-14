#ifndef ATA_H                
#define ATA_H

#include "system.h"
#include "scheduler/process.h"
#include "drivers/ata/ata.h"

#define READ_28 0x20
#define WRITE_28 0x30

#define READ(sector_count,lba,io_buffer) ext2->device_desc->write(ext2->device_desc,sector_count,lba,io_buffer);
#define WRITE(sector_count,lba,io_buffer) ext2->device_desc->write(ext2->device_desc,sector_count,lba,io_buffer);

void init_ata(t_device_desc* ata_desc);
//unsigned int _read_28_ata(t_device_desc* ata_desc,unsigned int sector_count,unsigned int lba,void* io_buffer);
//unsigned int _write_28_ata(t_device_desc* ata_desc,unsigned int sector_count,unsigned int lba,void* io_buffer);
unsigned int _read_28_ata(t_io_request* io_request);
unsigned int _write_28_ata(t_io_request* io_request);
void free_ata();

#endif
