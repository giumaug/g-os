#include "drivers/ioapic/ioapic.h"                
                     
static unsigned char* ioregsel = NULL;
static unsigned char* ioregwin = NULL;

static void write_reg(u32 reg_offset, u32 val);
static u32 read_reg(u32 reg_offset);

void init_ioapic()
{
	int i;
	u32 offset;
	u32 val;
	
	map_vm_mem(system.master_page_dir, IOREGSEL, IOREGSEL, PAGE_SIZE,3);
	ioregsel = IOREGSEL;
	ioregwin = IOREGWIN;
	for (i = 0; i < 46; i += 2) //23
	{
		//offset = 0x10 + 2 * i;
		offset = 16 + i;
		//val = REDTBL_LOW_ENTRY & (0x20 + (i / 2));
		val = (32 + (i / 2));
		write_reg(offset, val);
		offset++;
		write_reg(offset, REDTBL_HI_ENTRY);
	}
	//write_reg(16, 32);
	//write_reg(17, 0);
}

void unmask_entry(u8 offset)
{
	u32 _offset;
	u32 val;

	SAVE_IF_STATUS
	CLI
	_offset = (0x10 + 2 * offset);
	val = read_reg(_offset);
	val = val & (~0x10000);
	write_reg(_offset, val);
	RESTORE_IF_STATUS
}

void mask_entry(u8 offset)
{
	u32 _offset;
	u32 val;

	SAVE_IF_STATUS
	CLI
	_offset = (0x10 + 2 * offset);
	val = read_reg(_offset);
	val = val | 0x10000;
	write_reg(_offset, val);
	RESTORE_IF_STATUS
}

static void write_reg(u32 reg_offset, u32 val)
{
	*ioregsel = reg_offset;
	*((volatile u32*)(ioregwin)) = val;
}

static u32 read_reg(u32 reg_offset)
{
	u32 val;

	*ioregsel = reg_offset;
	val = *((volatile u32*)(ioregwin));
	return val;
}
