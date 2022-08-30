#ifndef AHCI_H                
#define AHCI_H

#include "system.h"

#define AHCI_PCI_BUS
#define AHCI_PCI_SLOT
#define AHCI_PCI_FUNC
#define AHCI_PORT_COUNT 1
#define AHCI_PCI_BAR5 0x24

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000


typedef struct s_fis_reg_h2d
{
	// DW0
	u8  cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
	u8  a:1;		// ATAPI
	u8  w:1;		// Write, 1: H2D, 0: D2H
	u8  p:1;		// Prefetchable
 
	u8  r:1;		// Reset
	u8  b:1;		// BIST
	u8  c:1;		// Clear busy upon R_OK
	u8  rsv0:1;		// Reserved
	u8  pmp:4;		// Port multiplier port
 
	u16 prdtl;		// Physical region descriptor table length in entries
 
	// DW1
	volatile
	uint32_t prdbc;		// Physical region descriptor byte count transferred
 
	// DW2, 3
	uint32_t ctba;		// Command table descriptor base address
	uint32_t ctbau;		// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	uint32_t rsv1[4];	// Reserved
}
t_fis_reg_h2d;



typedef struct s_hba_cmd_header
{
	//DW0
	u8 cfl;
	u8 a;
	u8 w;
	u8 p;
	u8 r;
	u8 b;
	u8 c;
	u8 rsv0;
	u8 pmp;
	u16 prdtl;
	//DW1
	u32 prdbc;
	//DW2
	u32 ctba;
	//DW3
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
	u32 dbc;
	u32 rsv1;
	u32 i;
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

typedef struct s_ahci_device
{
    struct s_device_desc* device;
    char* abar;
}
t_ahci_device;

t_device_desc* init_ahci(u8 device_num);
void free_ahci(t_device_desc* device_desc);
void int_handler_ahci()
u8 _write_dma_28_ahci(t_io_request* io_request);
u8 _read_dma_28_ahci(t_io_request* io_request);
u8 _read_28_ahci(t_io_request* io_request);
u8 _write_28_ahci(t_io_request* io_request);
u8 _p_read_28_ahci(t_io_request* io_request);
u8 _p_write_28_ahci(t_io_request* io_request);

#endif
