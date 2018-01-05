#ifndef ATA_H                
#define ATA_H

#include "system.h"
#include "scheduler/process.h"
#include "drivers/ata/ata.h"

//check from Linux
#define ATA_BUS 0x0??
#define ATA_SLOT 0x0
#define ATA_FUNC 0x0
#define ATA_CMD_REG 0x2
#define ATA_PRD_REG 0x4

#define ALIGN_DMA_BUFFER(address)                                                                                           \
					((address % 0x10000) != 0) ?                                                        \
					(((address + 0x10000) - ((address + 0x10000) % 0x10000))) : (address % 0x10000)    

#define READ_28 0x20
#define WRITE_28 0x30

void init_ata(t_device_desc* ata_desc);
unsigned int _read_28_ata(t_io_request* io_request);
unsigned int _write_28_ata(t_io_request* io_request);
unsigned int _p_read_28_ata(t_io_request* io_request);
unsigned int _p_write_28_ata(t_io_request* io_request);

void free_ata();
int _flush_ata_pending_request();

#endif
