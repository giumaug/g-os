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
