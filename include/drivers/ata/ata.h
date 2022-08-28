#ifndef ATA_H                
#define ATA_H

#include "device.h"
#include "system.h"
#include "scheduler/process.h"

#define ATA_PCI_BUS 0x0
#define ATA_PCI_SLOT 0x1
#define ATA_PCI_FUNC 0x1
#define ATA_PCI_BAR4 0x20
#define ATA_PCI_COMMAND 0x4
#define ATA_PCI_VIRT_BAR4_MEM 0x0
#define ATA_PCI_VIRT_BAR4_MEM_SIZE 0x0
#define ATA_DMA_COMMAND_REG 0x0
#define ATA_DMA_STATUS_REG 0x2
#define ATA_DMA_PRD_REG 0x4

#define READ_28 0x20
#define WRITE_28 0x30
#define READ_DMA_28 0xC8
#define WRITE_DMA_28 0xCA

#define DMA_BUFFER_SIZE 0x20000 

t_device_desc* init_ata(u8 device_num);
unsigned int _read_28_ata(t_io_request* io_request);
unsigned int _write_28_ata(t_io_request* io_request);
unsigned int _p_read_28_ata(t_io_request* io_request);
unsigned int _p_write_28_ata(t_io_request* io_request);
unsigned int _read_dma_28_ata(t_io_request* io_request);
unsigned int _write_dma_28_ata(t_io_request* io_request);

void free_ata();
int _flush_ata_pending_request();
void init_ata_test();

#endif
