#ifndef ATA_H                
#define ATA_H

#include "system.h"
#include "scheduler/process.h"
#include "drivers/ata/ata.h"

//check from Linux
#define ATA_PCI_BUS 0x0
#define ATA_PCI_SLOT 0x1
#define ATA_PCI_FUNC 0x1
#define ATA_PCI_BAR4 0x20
#define ATA_PCI_VIRT_BAR4_MEM 0x0
#define ATA_PCI_VIRT_BAR4_MEM_SIZE 0x0
#define ATA_DMA_COMMAND_REG 0x0
#define ATA_DMA_STATUS_REG 0x2
#define ATA_DMA_PRD_REG 0x4

#define ALIGN_DMA_BUFFER(address)                                                                                           \
					(((address % 0x10000) != 0) ?                                                        \
					(((address + 0x10000) - ((address + 0x10000) % 0x10000))) : (address % 0x10000))    

#define READ_28 0x20
#define WRITE_28 0x30
#define READ_DMA_28 0xC8
#define WRITE_DMA_28 0xCA

void init_ata(t_device_desc* ata_desc);
unsigned int _read_28_ata(t_io_request* io_request);
unsigned int _write_28_ata(t_io_request* io_request);
unsigned int _p_read_28_ata(t_io_request* io_request);
unsigned int _p_write_28_ata(t_io_request* io_request);
unsigned int _read_dma_28_ata(t_io_request* io_request);

void free_ata();
int _flush_ata_pending_request();

#endif
