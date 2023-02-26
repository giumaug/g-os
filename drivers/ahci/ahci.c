#include "drivers/lapic/lapic.h"
#include "drivers/ahci/ahci.h"

static void port_init(t_hba_port* port, t_hashtable* mem_map, u8 port_num);
static void port_free(t_hba_port* port, t_hashtable* mem_map);
static int check_type(t_hba_port* port);

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
t_ahci_device_desc* init_ahci(t_device_desc* device_desc)
{
    u8 i;
    u32 pci_command;
    u32 pci_int_line;
    char* phy_abar;
    t_hba_port* port = NULL;
    t_hba_mem* mem = NULL;
    t_ahci_device_desc* device_desc_ahci = NULL;
    struct t_i_desc i_desc;
    
    device_desc_ahci = kmalloc(sizeof(t_ahci_device_desc));
    device_desc_ahci->mem_map = hashtable_init(ALIGNED_MEM_MAP_SIZE);
    
    //Dovrebbe essere memory mapped. Per verificare controllare il valore letto da bar5 con quello resituito da lspci
	phy_abar = read_pci_config_word(AHCI_PCI_BUS, AHCI_PCI_SLOT, AHCI_PCI_FUNC, AHCI_PCI_BAR5);
	device_desc_ahci->abar = AHCI_VIRT_MEM;
	device_desc_ahci->mem = AHCI_VIRT_MEM; 
	//BUS MASTER BIT SET-UP (2 bit starting from 0)
	pci_command = read_pci_config_word(AHCI_PCI_BUS, AHCI_PCI_SLOT, AHCI_PCI_FUNC, AHCI_PCI_COMMAND);
    pci_command |= 0x4;
	write_pci_config_word(AHCI_PCI_BUS, AHCI_PCI_SLOT, AHCI_PCI_FUNC, AHCI_PCI_COMMAND, pci_command);
	// INTERRUPT LINE
	pci_int_line = read_pci_config_word(AHCI_PCI_BUS, AHCI_PCI_SLOT, AHCI_PCI_FUNC, AHCI_PCI_INT_LINE);
    pci_int_line |= 0x1;
	write_pci_config_word(AHCI_PCI_BUS, AHCI_PCI_SLOT, AHCI_PCI_FUNC, AHCI_PCI_INT_LINE, pci_int_line);
    map_vm_mem(system.master_page_dir, AHCI_VIRT_MEM, ((u32) (phy_abar)), AHCI_VIRT_MEM_SIZE, 3);
    
    device_desc_ahci->mem->ghc = device_desc_ahci->mem->ghc | 2;
    port = &(device_desc_ahci->mem->ports[0]);
    port_init(port, device_desc_ahci->mem_map, 0);
    device_desc_ahci->active_port = port;
    
	i_desc.baseLow = ((int) &int_handler_ahci) & 0xFFFF;
	i_desc.selector = 0x8;
	i_desc.flags = 0x08e00;
	i_desc.baseHi = ((int) &int_handler_ahci) >> 0x10;
	set_idt_entry(0x31, &i_desc);
	
	device_desc->dev = device_desc_ahci;
	device_desc->read_dma = _read_28_ahci;
	device_desc->write_dma = _write_28_ahci;
	device_desc->read = _read_28_ahci;
	device_desc->write = _write_28_ahci;
	device_desc->p_read = _p_read_28_ahci;
	device_desc->p_write = _p_write_28_ahci;
}

void free_ahci(t_ahci_device_desc* device_desc)
{
    t_hba_port* port = NULL;
    
    port = &(device_desc->mem->ports[0]);
    port_free(port, 0);
    umap_vm_mem(system.master_page_dir, AHCI_VIRT_MEM, AHCI_VIRT_MEM_SIZE, 0);
    kfree(device_desc);
}

void int_handler_ahci()
{	
	struct t_processor_reg processor_reg;
	t_io_request* io_request = NULL;
	struct t_process_context* process_context = NULL;
	struct t_process_context* current_process_context = NULL;
	
	t_ahci_device_desc* ahci_device_desc = NULL;
	t_hba_port* port = NULL;

	SAVE_PROCESSOR_REG
	SWITCH_DS_TO_KERNEL_MODE
	DISABLE_PREEMPTION
	mask_entry(17);
	EOI_TO_LAPIC
	STI
	CURRENT_PROCESS_CONTEXT(current_process_context);
	
	io_request = system.device_desc->serving_request;
	process_context = io_request->process_context;

	if (system.device_desc->status == DEVICE_BUSY)
	{
		sem_up(&system.device_desc->sem);
	}
	if (current_process_context->pid != process_context->pid) 
	{
	 	system.force_scheduling = 1;
	}
	
	port = ((t_ahci_device_desc*) system.device_desc->dev)->active_port;
	port->is = 1;
	ahci_device_desc = system.device_desc->dev;
	ahci_device_desc->mem->is = 1;
	
	unmask_entry(17);
	ENABLE_PREEMPTION
	EXIT_INT_HANDLER(0,processor_reg)
}

static s8 __read_write_28_ahci(t_io_request* io_request)
{
	t_device_desc* device_desc = NULL;
    t_hba_port* port = NULL;
    t_hba_cmd_header* cmd_header = NULL;
    t_hba_cmd_tbl* cmd_tbl = NULL;
    t_fis_reg_h2d* cmd_fis = NULL;
    
    device_desc = io_request->device_desc;
    port = ((t_ahci_device_desc*) device_desc->dev)->active_port;
  
    port->is = (u32) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmd_slot(port);
	if (slot == -1)
	{
		return -1;
    }
    cmd_header = FROM_PHY_TO_VIRT(port->clb);
    io_request->command == ATA_READ_DMA_28 ? 0 : 1;
    cmd_header += slot;
	cmd_header->cfl = sizeof(t_fis_reg_h2d) / sizeof(u32);
	cmd_header->w = (io_request->command == ATA_READ_DMA_28 ? 0 : 1);
	
	cmd_tbl = FROM_PHY_TO_VIRT(cmd_header->ctba);
	cmd_tbl->prdt_entry[0].dba = FROM_VIRT_TO_PHY(io_request->io_buffer);
	cmd_tbl->prdt_entry[0].dbau = 0;
	cmd_tbl->prdt_entry[0].dbc = (io_request->sector_count * AHCI_SECTOR_SIZE) - 1;
	cmd_tbl->prdt_entry[0].i = 1;
	
	cmd_fis = cmd_tbl->cfis;
	cmd_fis->fis_type = FIS_TYPE_REG_H2D;
	cmd_fis->c = 1;
	cmd_fis->command = io_request->command;
	
	cmd_fis->lba0 = (unsigned char) io_request->lba;
	cmd_fis->lba1 = (unsigned char)(io_request->lba >> 8);
	cmd_fis->lba2 = (unsigned char)(io_request->lba >> 16);
	cmd_fis->lba3 = 0;
	cmd_fis->lba4 = 0;
	cmd_fis->lba5 = 0;
	cmd_fis->device = 1<<6;
	cmd_fis->countl = io_request->sector_count & 0xFF;
	cmd_fis->counth = (io_request->sector_count >> 8) & 0xFF;
	
	//WAIT PORT IS READ AND ISSUE THE COMMAND
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < ATA_SPIN_TIMEOUT)
	{
		spin++;
	}
	if (spin == ATA_SPIN_TIMEOUT)
	{
		return -1;
	}
	port->ci = 1 << slot;
	return slot;
}

static s8 _p_read_write_28_ahci(t_io_request* io_request)
{
	s8 ret = 0;
	int slot;
	t_hba_port* port = NULL;
	t_device_desc* device_desc = NULL;
	t_spinlock_desc spinlock;
	
	SPINLOCK_INIT(spinlock);
	//Entrypoint mutual exclusion region.
	SPINLOCK_LOCK(spinlock);
	device_desc = io_request->device_desc;
	device_desc->status = DEVICE_BUSY || POOLING_MODE;
    port = ((t_ahci_device_desc*) device_desc->dev)->active_port;
	slot = __read_write_28_ahci(io_request);
	if (slot == 1)
	{
			ret = -1;
			goto EXIT;
	}
	
	// Wait for completion
	while (1)
	{
		if ((port->ci & (1 << slot)) == 0) break;
		if (port->is & HBA_PxIS_TFES)
		{
			ret = -1;
		}
	}
	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		ret = -1;
	}
EXIT:
	device_desc->status = DEVICE_IDLE;
	//Exitpoint mutual exclusion region
	SPINLOCK_UNLOCK(spinlock);
	return ret;
}

static u8 _read_write_28_ahci(t_io_request* io_request)
{
	s8 ret;
	t_hba_port* port = NULL;
	t_device_desc* device_desc = NULL;
	
	device_desc = io_request->device_desc;
	//Entrypoint mutual exclusion region.
	//Here all requestes are enqueued using semaphore internal queue.
	//In order to implement a multilevel disk scheduler a better design
	//is to use an external queue with multilevel priority slots.
	sem_down(&device_desc->mutex);
	device_desc->status = DEVICE_BUSY;
    port = ((t_ahci_device_desc*) device_desc->dev)->active_port;
	ret = __read_write_28_ahci(io_request);
	
	//semaphore to avoid race with interrupt handler
	sem_down(&device_desc->sem);
	device_desc->status = DEVICE_IDLE;
	//Endpoint mutual exclusion region
	sem_up(&device_desc->mutex);
	return ret;
}

u8 _read_28_ahci(t_io_request* io_request)
{
	io_request->command = ATA_READ_DMA_28;
	return _read_write_28_ahci(io_request);	
}

u8 _write_28_ahci(t_io_request* io_request)
{
	io_request->command = ATA_WRITE_DMA_28;
	return _read_write_28_ahci(io_request);

}

u8 _p_read_28_ahci(t_io_request* io_request)
{
	io_request->command = ATA_READ_DMA_28;
	return _p_read_write_28_ahci(io_request);		
}

u8 _p_write_28_ahci(t_io_request* io_request)
{
	io_request->command = ATA_WRITE_DMA_28;
	return _p_read_write_28_ahci(io_request);
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

static void port_init(t_hba_port* port, t_hashtable* mem_map, u8 port_num)
{
	int i;
    //command list 1024 aligned
    //command table 128 aligned
    //received fis 256  aligned
    char* addr = NULL;
    char* algnd_addr = NULL;
    t_hba_cmd_header* cmd_header = NULL;
    
    stop_cmd(port);
    addr = kmalloc(1024 + 1024);  
    algnd_addr = ALIGNED_TO_OFFSET(addr, 1024);
    port->clb = FROM_VIRT_TO_PHY((u32) algnd_addr);
    port->clbu = 0; // to check address size
    kfillmem(algnd_addr, 0, 1024);
    hashtable_put(mem_map, algnd_addr, addr);
    
    addr = kmalloc(256 + 256);
    algnd_addr = ALIGNED_TO_OFFSET(addr, 256);
    port->fb = FROM_VIRT_TO_PHY((u32) algnd_addr);
    port->fbu = 0; // to check address size
    kfillmem(algnd_addr, 0, 256);
    hashtable_put(mem_map, algnd_addr, addr);
    
    cmd_header = FROM_PHY_TO_VIRT(port->clb);
    for (i = 0; i < 32; i++)
	{
	    // 1 prdt entry per command table
	    // 256 bytes per command table, 64+16+48+128;
        cmd_header[i].prdtl = 1;
        addr = kmalloc(256 + 256);
        algnd_addr = ALIGNED_TO_OFFSET(addr, 256);
        cmd_header[i].ctba = FROM_VIRT_TO_PHY((u32) algnd_addr);
        cmd_header[i].ctbau = 0;
        kfillmem(algnd_addr, 0, 256);
        hashtable_put(mem_map, algnd_addr, addr);
    }
    port->ie = 1;
    start_cmd(port);   
}

static void port_free(t_hba_port* port, t_hashtable* mem_map)
{
	int i;
	char* addr = NULL;
    char* algnd_addr = NULL;
	t_hba_cmd_header* cmd_header = NULL;
	
	stop_cmd(port);
	cmd_header = port->clb;
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

void test_ahci()
{
	int i;
    char* io_buffer = NULL;
    t_io_request* io_request = NULL;
    int partition_start_sector = 51400;
    
    io_buffer = aligned_kmalloc(BLOCK_SIZE, 16);
    
    for (i = 0; i < 512; i++)
	{
			io_buffer[i] = 'k';
	}
    
    io_request = kmalloc(sizeof(t_io_request));
    io_request->device_desc = system.device_desc;
    io_request->sector_count= 1;
    //io_request->lba= 2 + partition_start_sector;
    io_request->lba= partition_start_sector;
    io_request->io_buffer = io_buffer;
    io_request->process_context = NULL;
    _write_28_ahci(io_request);
    kfree(io_request);
    aligned_kfree(io_buffer);                                                      					
}

static int check_type(t_hba_port* port)
{
	u32 ssts = port->ssts;
 
	u8 ipm = (ssts >> 8) & 0x0F;
	u8 det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}
        
    
    
    
    
    










