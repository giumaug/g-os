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

//WE CONSIDER A DEVICE WITH ONE PORT ONLY
t_ahci_device_desc* init_ahci(u8 device_num)
{
    u8 i;
    char* phy_abar;
    t_hba_port* port = NULL;
    t_ahci_device_desc* device_desc = NULL;
    
    device_desc = kmalloc(sizeof(t_ahci_device_desc));
    device_desc->device = init_device(device_num);
    device_desc->mem_map = hashtable_init(ALIGNED_MEM_MAP_SIZE);
    
    //Dovrebbe essere memory mapped. Per verificare controllare il valore letto da bar5 con quello resituito da lspci
	phy_abar = read_pci_config_word(ATA_PCI_BUS, ATA_PCI_SLOT, ATA_PCI_FUNC, ATA_PCI_BAR5);
	device_desc->abar = AHCI_VIRT_MEM;
	//BUS MASTER BIT SET-UP (2 bit starting from 0)
	pci_command = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND);
    pci_command |= 0x4;
	write_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND,pci_command);
	//pci_command = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND);
	
    map_vm_mem(system.master_page_dir, AHCI_VIRT_MEM, ((u32) (phy_abar)), AHCI_VIRT_MEM_SIZE, 3);
    port = device_desc->abar + (128 * 0);
    port_init(port, device_desc->mem_map, 0);
    device_desc->active_port = port;
}

void free_ahci(t_ahci_device_desc* device_desc)
{
    port = device_desc->abar + (128 * 0);
    port_free(port, 0);
    umap_vm_mem(process_context->page_dir, AHCI_VIRT_MEM, AHCI_VIRT_MEM_SIZE, 0);
    free_device(device_desc->device);
    kfree(device_desc);
}

void int_handler_ahci()
{
	
}

//--------------nuove---------------

static u8 _read_write_ahci(t_io_request* io_request)
{
    t_ahci_device_desc* device_desc = NULL;
    t_hba_port* port = NULL;
    t_hba_cmd_header* cmd_header = NULL;

    device_desc = io_request->device_desc;
    port = device_desc->active_port;
    
    port->is = (u32) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmd_slot(port);
	if (slot == -1)
	{
		return 0;
    }
    cmd_header = port->clb;
    io_request->command == READ_DMA_28 || io_request->command == READ_28 ? 0 : 1;
    cmd_header += slot;
	cmd_header->cfl = sizeof(t_fis_reg_h2d) / sizeof(u32);
	cmd_header->w = (io_request->command == READ_DMA_28 || io_request->command == READ_28 ? 0 : 1);
	//cmd_header->prdtl = 1;--------qui!!!!!!!!!!!!!!!!
 
	
    
	
}

static u8 _p_read_write_ahci(t_io_request* io_request)
{
	
}

//--------------------------------------

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

static void port_init(u32 abar, t_hba_port port, t_hashtable* mem_map, u8 port_num)
{
    //command list 1024 aligned
    //command table 128 aligned
    //received fis 256  aligned
    char* addr = NULL;
    char* algnd_addr = NULL;
    t_hba_cmd_header* cmd_header = NULL;
    
    stop_cmd(port);
    addr = kmalloc(1024 + 1024);  
    algnd_addr = ALIGNED_TO_OFFSET(addr, 1024);
    port->clb = FROM_VIRT_TO_PHY((u32) addr);
    port->clbu = 0; // to check address size
    kfillmem(addr, 0, 1024);
    hashtable_put(mem_map, algnd_addr, addr);
    
    addr = kmalloc(256 + 256);
    algnd_addr = ALIGNED_TO_OFFSET(addr, 256);
    port->fb = FROM_VIRT_TO_PHY((u32) addr);
    port->fbu = 0; // to check address size
    kfillmem(addr, 0, 256);
    hashtable_put(mem_map, algnd_addr, addr);
    
    cmd_header = port->clb;
    for (i = 0; i < 32; i++)
	{
	    // 1 prdt entry per command table
	    // 144 bytes per command table, 64+16+48+16;
        cmd_header[i].prdtl = 1;
        addr = kmalloc(144 + 144);
        addr = ALIGNED_TO_OFFSET(addr, 144);
        cmd_header[i].ctba = FROM_VIRT_TO_PHY((u32) addr);
        cmd_header[i].ctbau = 0;
        kfillmem(addr, 0, 144);
        hashtable_put(mem_map, algnd_addr, addr);
    }
    stop_cmd(port);   
}

static void port_free(u32 abar, t_hba_port port, t_hashtable* mem_map)
{
	int i;
	char* addr = NULL;
    char* algnd_addr = NULL;
	t_hba_cmd_header* cmd_header = NULL;
	
	stop_cmd(port);
	cmdheader = port->clb;
    for (i = 0; i < 32; i++)
	{
		algnd_addr = FROM_PHY_TO_VIRT(cmd_header[i].ctba);
		addr = hashtable_remove(mem_map, algnd_addr);
		kfree(addr);
    }
	algnd_addr = FROM_PHY_TO_VIRT(port->clb);
	addr = hashtable_remove(mem_map, algnd_addr);
	kfree(addr);
	
	algnd_addr = FROM_PHY_TO_VIRT(port->fb);
	addr = hashtable_remove(mem_map, algnd_addr);
	kfree(addr);
}  
        
    
    
    
    
    










