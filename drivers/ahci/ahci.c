#include "drivers/ahci/ahci.h"

int static find_cmd_slot(t_hba_port* port)
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

t_device_desc* init_ahci(u8 device_num)
{
    //Dovrebbe essere memory mapped. Per verificare controllare il valore letto da bar5 con quello resituito da lspci
	bar4 = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_BAR4);
	//BUS MASTER BIT SET-UP (2 bit starting from 0)
	pci_command = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND);
    pci_command |= 0x4;
	write_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND,pci_command);
	//pci_command = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND);
}

void free_ahci(t_device_desc* device_desc)
{
	
}

void int_handler_ahci()
{
	
}

static u8 _read_write_dma_28_ahci(t_io_request* io_request)
{
	
}

static u8 _read_write_28_ahci(t_io_request* io_request)
{
	
}

u8 _write_dma_28_ahci(t_io_request* io_request)
{
	
}

u8 _read_dma_28_ahci(t_io_request* io_request)
{

}

u8 _read_28_ahci(t_io_request* io_request)
{

}

u8 _write_28_ahci(t_io_request* io_request)
{
	
}

u8 _p_read_28_ahci(t_io_request* io_request)
{
		
}

u8 _p_write_28_ahci(t_io_request* io_request)
{

}






