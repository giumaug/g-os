typedef struct s_fis_reg_h2d
{
	//DW0
	u8 fis_type;
	u8 pmport;
	u8 rsv0;
	u8 c;
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
	u8  countl;
	u8  counth;
	u8  icc;
	u8  control;
	//DW4
	u8  rsv1[4];
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


typedef volatile struct tagHBA_PORT
{
	uint32_t clb;		// 0x00, command list base address, 1K-byte aligned
	uint32_t clbu;		// 0x04, command list base address upper 32 bits
	uint32_t fb;		// 0x08, FIS base address, 256-byte aligned
	uint32_t fbu;		// 0x0C, FIS base address upper 32 bits
	uint32_t is;		// 0x10, interrupt status
	uint32_t ie;		// 0x14, interrupt enable
	uint32_t cmd;		// 0x18, command and status
	uint32_t rsv0;		// 0x1C, Reserved
	uint32_t tfd;		// 0x20, task file data
	uint32_t sig;		// 0x24, signature
	uint32_t ssts;		// 0x28, SATA status (SCR0:SStatus)
	uint32_t sctl;		// 0x2C, SATA control (SCR2:SControl)
	uint32_t serr;		// 0x30, SATA error (SCR1:SError)
	uint32_t sact;		// 0x34, SATA active (SCR3:SActive)
	uint32_t ci;		// 0x38, command issue
	uint32_t sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	uint32_t fbs;		// 0x40, FIS-based switch control
	uint32_t rsv1[11];	// 0x44 ~ 0x6F, Reserved
	uint32_t vendor[4];	// 0x70 ~ 0x7F, vendor specific
} HBA_PORT;

int find_cmd_slot(t_hba_port* port)
{
	u32 slots = (port->sact | port->ci);
	for (int i = 0; i < 31; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	return -1;
}






