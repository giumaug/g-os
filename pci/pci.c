#include "pci.h" 

u16 read_pci_config_word(u8 bus, u8 slot,u8 func,u8 offset)
{
	u32 address;
	u32 lbus  = (uint32_t)bus;
	u32 lslot = (uint32_t)slot;
	u32 lfunc = (uint32_t)func;
 
	address = (u32)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((u32)0x80000000));
 
	out(address,0xCF8);
    	return (u16)((in(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
}
