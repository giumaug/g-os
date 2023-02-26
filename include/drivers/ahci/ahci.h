#ifndef AHCI_H                
#define AHCI_H

#include "system.h"
#define AHCI_SECTOR_SIZE 512
#define ALIGNED_MEM_MAP_SIZE 100
//ABAR NEEDS 4352 BYTES
#define AHCI_VIRT_MEM      0x90000
#define AHCI_VIRT_MEM_SIZE 0x1400
#define AHCI_PCI_BUS 0x0
#define AHCI_PCI_SLOT 0x1F
#define AHCI_PCI_FUNC 0x02
#define AHCI_PORT_COUNT 1
#define AHCI_PCI_BAR5 0x24
#define AHCI_PCI_INT_LINE 0x3C
#define AHCI_PCI_COMMAND 0x4

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

#define HBA_PxIS_TFES 0x40000000

#define FIS_TYPE_REG_H2D 0x27
#define ATA_READ_DMA_28 0xC8
#define ATA_WRITE_DMA_28 0xCA
//#define ATA_WRITE_DMA_28 0x35
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define ATA_SPIN_TIMEOUT 1000000

//-----------------------------
#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier
 
#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4
 
#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define DMA_BUFFER_SIZE 0x10000
//------------------------------------

typedef struct s_fis_reg_h2d
{
	//DW0
    u8 fis_type;
 
	u8 pmport:4;	
	u8 rsv0:3;		
	u8 c:1;		
 
	u8 command;	
	u8 featurel;	
 
	//DW1
	u8 lba0;	
	u8 lba1;		
	u8 lba2;		
	u8 device;
 
	//DW2
	u8 lba3;
	u8 lba4;
	u8 lba5;
	u8 featureh;
 
	//DW3
	u8 countl;
	u8 counth;
	u8 icc;
	u8 control;
 
	//DW4
	u8 rsv1[4];
}
t_fis_reg_h2d;



typedef struct s_hba_cmd_header
{
	//DW0
	u8 cfl:5;
	u8 a:1;
	u8 w:1;
	u8 p:1;
 
	u8 r:1;
	u8 b:1;
	u8 c:1;
	u8 rsv0:1;
	u8 pmp:4;
 
	u16 prdtl;
 
	//DW1
	u32 prdbc;
 
	//DW2, 3
	u32 ctba;
	u32 ctbau;
 
	//DW4 - 7
	u32 rsv1[4];
}
t_hba_cmd_header;

typedef struct s_hba_prdt_entry
{
	u32 dba;
	u32 dbau;
	u32 rsv0;
 
	//DW3
	u32 dbc:22;
	u32 rsv1:9;
	u32 i:1;
}
t_hba_prdt_entry;

typedef struct s_hba_cmd_tbl
{
	u8  cfis[64];
	u8  acmd[16];
	u8  rsv[48];
	t_hba_prdt_entry prdt_entry[1];
}
t_hba_cmd_tbl;

typedef struct s_hba_port
{
	u32 clb;
	u32 clbu;
	u32 fb;
	u32 fbu;
	u32 is;
	u32 ie;
	u32 cmd;
	u32 rsv0;
	u32 tfd;
	u32 sig;
	u32 ssts;
	u32 sctl;
	u32 serr;
	u32 sact;
	u32 ci;
	u32 sntf;
	u32 fbs;
	u32 rsv1[11];
	u32 vendor[4];
}
t_hba_port;

typedef struct s_hba_mem
{
	u32 cap;
	u32 ghc;
	u32 is;
	u32 pi;
	u32 vs;
	u32 ccc_ctl;
	u32 ccc_pts;
	u32 em_loc;
	u32 em_ctl;
	u32 cap2;
	u32 bohc;
	u8 rsv[0xA0-0x2C];
	u8 vendor[0x100-0xA0];
	t_hba_port ports[32];
}
t_hba_mem;

typedef struct s_ahci_device_desc
{
    struct s_device_desc* device;
    char* abar;
    t_hba_mem* mem;
    t_hba_port* active_port;
    t_hashtable* mem_map;
}
t_ahci_device_desc;

t_ahci_device_desc* init_ahci(t_device_desc* device_desc);
void free_ahci(t_ahci_device_desc* device_desc);
void int_handler_ahci();
u8 _write_28_ahci(t_io_request* io_request);
u8 _read_28_ahci(t_io_request* io_request);
u8 _read_28_ahci(t_io_request* io_request);
u8 _p_read_28_ahci(t_io_request* io_request);
u8 _p_write_28_ahci(t_io_request* io_request);
void test_ahci();
#endif
