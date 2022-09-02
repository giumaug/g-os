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

t_ahci_device_desc* init_ahci(u8 device_num)
{
    u8 i;
    char* phy_abar;
    t_hba_port* port = NULL;
    t_ahci_device_desc* device_desc = NULL;
    
    device_desc = kmalloc(sizeof(t_ahci_device_desc));
    device_desc->device = init_device(device_num);
    
    //Dovrebbe essere memory mapped. Per verificare controllare il valore letto da bar5 con quello resituito da lspci
	phy_abar = read_pci_config_word(ATA_PCI_BUS, ATA_PCI_SLOT, ATA_PCI_FUNC, ATA_PCI_BAR5);
	device_desc->abar = AHCI_VIRT_MEM;
	//BUS MASTER BIT SET-UP (2 bit starting from 0)
	pci_command = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND);
    pci_command |= 0x4;
	write_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND,pci_command);
	//pci_command = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND);
	
    map_vm_mem(system.master_page_dir, AHCI_VIRT_MEM, ((u32) (phy_abar)), AHCI_VIRT_MEM_SIZE, 3);
	for (i = 1; i <= AHCI_PORT_COUNT; i++)
    {
        port = device_desc->abar + (128 * i);
        port_init(port, i);
    }
}

void free_ahci(t_ahci_device_desc* device_desc)
{
    for (i = 1; i <= AHCI_PORT_COUNT; i++)
    {
        port = device_desc->abar + (128 * i);
        port_free(port, i);
    }
    CURRENT_PROCESS_CONTEXT(current_process_context);
    umap_vm_mem(process_context->page_dir, AHCI_VIRT_MEM, AHCI_VIRT_MEM_SIZE,0);
	SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir)
    
    free_device(device_desc->device);
    kfree(device_desc);
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

void start_cmd(t_hba_port* port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stop_cmd(t_hba_port* port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
 
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
}

static void port_init(u32 abar, t_hba_port port, u8 port_num)
{
    //command list 1024 aligned
    //command table 128 aligned
    //received fis 256  aligned
    char* addr;
    t_hba_cmd_header* cmd_header = NULL;
    
    stop_cmd(port);
    addr = kmalloc(1024 + 1024);
    addr = ALIGNED_TO_OFFSET(addr, 1024);
    port->clb = FROM_VIRT_TO_PHY((u32) addr);
    port->clbu = 0; // to check address size
    kfillmem(addr, 0, 1024);
    
    addr = kmalloc(256 + 256);
    addr = ALIGNED_TO_OFFSET(addr, 256);
    port->fb = FROM_VIRT_TO_PHY((u32) addr);
    port->fbu = 0; // to check address size
    kfillmem(addr, 0, 256);
    
    cmdheader = port->clb;
    for (i = 0; i < 32; i++)
	{
	    // 8 prdt entries per command table
	    // 256 bytes per command table, 64+16+48+16*8
        cmd_header[i].prdtl = 8;
        addr = kmalloc(256 + 256);
        addr = ALIGNED_TO_OFFSET(addr, 256);
        cmd_header[i].ctba = FROM_VIRT_TO_PHY((u32) addr);
        cmd_header[i].ctbau = 0;
        kfillmem(addr, 0, 256);
    }
    stop_cmd(port);   
}

static void port_free()
{

}  
        
    
    
    
    
    










