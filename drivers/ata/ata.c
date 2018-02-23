#include "common.h"
#include "asm.h" 
#include "idt.h"
#include "virtual_memory/vm.h"
#include "pci/pci.h"
#include "drivers/ata/ata.h"

//static int race=0;
//extern int go;
//extern int ggo;
//extern int pp1;
//extern int pp2;

void static int_handler_ata();

u32 dma_pci_io_base;
u32 dma_pci_mem_base;
u8 dma_pci_bar_type;

static u8 read_ata_config_byte(t_device_desc* device_desc,u32 address)
{
	u32 virt_addr;
	u8 value;

	if (device_desc->dma_pci_bar_type == 0)
	{
		virt_addr = (u32) device_desc->dma_pci_mem_base + address;
		value =  (*((volatile u8*)(virt_addr)));
	}
	else 
	{
		value = in(device_desc->dma_pci_io_base + address);
	}
	return value;
}

static u32 read_ata_config_dword(t_device_desc* device_desc,u32 address)
{
	u32 virt_addr;
	u32 value;

	if (device_desc->dma_pci_bar_type == 0)
	{
		virt_addr = (u32) device_desc->dma_pci_mem_base + address;
		value =  (*((volatile u32*)(virt_addr)));
	}
	else 
	{
		value = indw(device_desc->dma_pci_io_base + address);
	}
	return value;
}

static void write_ata_config_dword(t_device_desc* device_desc,u8 address,u32 value)
{
	u32 virt_addr;
	u8 reg_value;

	if (device_desc->dma_pci_bar_type == 0)
	{
		virt_addr = (u32) device_desc->dma_pci_mem_base + address;
		 (*((volatile u32*)(virt_addr))) = (value);
	}
	else 
	{
		outdw(value,device_desc->dma_pci_io_base + address);
	}
}

static void write_ata_config_byte(t_device_desc* device_desc,u32 address,u8 value)
{
	u32 virt_addr;

	if (device_desc->dma_pci_bar_type == 0)
	{
		virt_addr = (u32) device_desc->dma_pci_mem_base + address;
		 (*((volatile u32*)(virt_addr))) = (value);
	}
	else 
	{
		out(value,device_desc->dma_pci_io_base + address);
	}
}

void init_ata(t_device_desc* device_desc)
{	
	struct t_i_desc i_desc;
	u32 bar4 = NULL;
	u32 pci_command = NULL;
	struct t_process_context* current_process_context = NULL;
	
	i_desc.baseLow = ((int)&int_handler_ata) & 0xFFFF;
	i_desc.selector = 0x8;
	i_desc.flags = 0x08e00;
	i_desc.baseHi = ((int)&int_handler_ata)>>0x10;
	set_idt_entry(0x2E,&i_desc);

	device_desc->read_dma = _read_dma_28_ata;
	device_desc->read = _read_28_ata;
	device_desc->write = _write_28_ata;
	device_desc->p_read = _p_read_28_ata;
	device_desc->p_write = _p_write_28_ata;
	device_desc->status = DEVICE_IDLE;
	sem_init(&device_desc->mutex,1);
	sem_init(&device_desc->sem,0);
	bar4 = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_BAR4);
	pci_command = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND);
	//printk("command reg is %x \n",pci_command);
     	pci_command |= 0x4;
	write_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND,pci_command);
	pci_command = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND);
	//printk("new command reg is %x \n",pci_command);

	if ((u32) bar4 & 0x1) 
	{
		device_desc->dma_pci_io_base = (u32) bar4 & 0xFFFC;
		device_desc->dma_pci_mem_base = NULL;
		device_desc->dma_pci_bar_type = 1;
	}
	else 
	{
		device_desc->dma_pci_mem_base = ATA_PCI_VIRT_BAR4_MEM;
		device_desc->dma_pci_io_base = NULL;
		device_desc->dma_pci_bar_type = 0;
		CURRENT_PROCESS_CONTEXT(current_process_context);
	map_vm_mem(current_process_context->page_dir,ATA_PCI_VIRT_BAR4_MEM,(((u32) (bar4)) & 0xFFFFFFF0),ATA_PCI_VIRT_BAR4_MEM_SIZE,3);
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) current_process_context->page_dir))) 
	}
}

void free_ata(t_device_desc* device_desc)
{
	device_desc->status=DEVICE_IDLE;
	sem_down(&device_desc->mutex);
}

void int_handler_ata()
{
	struct t_processor_reg processor_reg;
	t_io_request* io_request;
	struct t_process_context* process_context;
	struct t_process_context* current_process_context;

	SAVE_PROCESSOR_REG
	disable_irq_line(14);
	DISABLE_PREEMPTION
	EOI_TO_SLAVE_PIC
	EOI_TO_MASTER_PIC
	STI
	
	io_request = system.device_desc->serving_request;
	process_context = io_request->process_context;
	CURRENT_PROCESS_CONTEXT(current_process_context);

	if (system.device_desc->status != POOLING_MODE)
	{
		sem_up(&io_request->device_desc->sem);
	}
	if (current_process_context->pid != process_context->pid) 
	{
	 	system.force_scheduling = 1;
	}
	system.preempt_network_flush = 1;
	system.device_desc->status=DEVICE_IDLE;
	enable_irq_line(14);
	ENABLE_PREEMPTION
	EXIT_INT_HANDLER(0,processor_reg)
}

static unsigned int _read_write_dma_28_ata(t_io_request* io_request)
{
	int i;
	t_device_desc* device_desc = NULL;
	t_io_request* pending_request = NULL;
	t_llist_node* node = NULL;
	char* prd = NULL;
	char* aligned_prd = NULL;
	short byte_count;
	char is_last_prd;
	int ret = 0;
	t_llist_node* next = NULL;
	t_llist_node* first = NULL;
	t_dma_lba* dma_lba = NULL;
	u32 mem_addr;
	u8 dma_status;
	u8 cmd_status;
	
	device_desc = io_request->device_desc;
	//Entrypoint mutual exclusion region.
	//Here all requestes are enqueued using semaphore internal queue.
	//In order to implement a multilevel disk scheduler a better design
	//is to use an external queue with multilevel priority slots.
	sem_down(&device_desc->mutex);
	device_desc->status=DEVICE_BUSY;
	system.device_desc->serving_request=io_request;

	first = ll_first(io_request->dma_lba_list);
	next = first;
	prd = kmalloc(16 * io_request->dma_lba_list_size);
	aligned_prd = ((((u32)prd % 0x10) != 0) ? ((((u32)prd + 0x10) - (((u32)prd + 0x10) % 0x10))) : ((u32)prd % 0x10));   
	for (i = 0; i < io_request->dma_lba_list_size;i++)
	{
		if (i == io_request->dma_lba_list_size - 1)
		{
			is_last_prd = 0x80;	
		}
		else
		{
			is_last_prd = 0;
		}
		dma_lba = next->val;
		
		//ALIGN_TO_BOUNDARY(boundary,address) 
		mem_addr = FROM_VIRT_TO_PHY(ALIGN_TO_BOUNDARY(0x10000,(u32)dma_lba->io_buffer));
		byte_count = dma_lba->sector_count * SECTOR_SIZE;

		aligned_prd[(i * 8)] = LOW_OCT_32(mem_addr);
		aligned_prd[(i * 8) + 1] = MID_RGT_OCT_32(mem_addr);
		aligned_prd[(i * 8) + 2] = MID_LFT_OCT_32(mem_addr); 
		aligned_prd[(i * 8) + 3] = HI_OCT_32(mem_addr);
		aligned_prd[(i * 8) + 4] = LOW_16(byte_count);
		aligned_prd[(i * 8) + 5] = HI_16(byte_count); 
		aligned_prd[(i * 8) + 6] = 0;
		aligned_prd[(i * 8) + 7] = is_last_prd;
		
		next = ll_next(next);
	}
	write_ata_config_dword(device_desc,ATA_DMA_PRD_REG,FROM_VIRT_TO_PHY(aligned_prd));
	write_ata_config_byte(device_desc,ATA_DMA_STATUS_REG,0x6);
	next = first;
	
	if (io_request->dma_lba_list_size>1)
	{
		printk("www \b");
	}

	for (i = 0; i < io_request->dma_lba_list_size;i++)
	{
		dma_lba = next->val;
		out(0xE0 | (unsigned char)(dma_lba->lba >> 24),0x1F6);
		out((unsigned char)dma_lba->sector_count,0x1F2);
		out((unsigned char)dma_lba->lba,0x1F3);
		out((unsigned char)(dma_lba->lba >> 8),0x1F4);
		out((unsigned char)(dma_lba->lba >> 16),0x1F5);
		out(io_request->command,0x1F7);
		next = ll_next(next);
//	} //original end!!!!
	if (i == 0) write_ata_config_byte(device_desc,ATA_DMA_COMMAND_REG,0x1);
	//semaphore to avoid race with interrupt handler
	sem_down(&device_desc->sem);
	if (i == io_request->dma_lba_list_size-1)
	{
	write_ata_config_byte(device_desc,ATA_DMA_COMMAND_REG,0x0);	
	dma_status = read_ata_config_byte(device_desc,ATA_DMA_STATUS_REG);
	cmd_status = in(0x1F7);
	if ((cmd_status & 1) || dma_status & 1)
	{
		device_desc->status=DEVICE_IDLE;
		panic();//only for debugger
		ret = -1;
	}
	}
	} // new for end!!!!!!!!!!!!!!!!!!!!!!!!
	//Endpoint mutual exclusion region
	sem_up(&device_desc->mutex);
	kfree(prd);	
	return ret;
}

static unsigned int _read_write_28_ata(t_io_request* io_request)
{
	int i;
	t_device_desc* device_desc = NULL;
	t_io_request* pending_request = NULL;
	t_llist_node* node = NULL;
	int k = 0;
	int s;
	
	device_desc = io_request->device_desc;
	//Entrypoint mutual exclusion region.
	//Here all requestes are enqueued using semaphore internal queue.
	//In order to implement a multilevel disk scheduler a better design
	//is to use an external queue with multilevel priority slots.
	sem_down(&device_desc->mutex);
	device_desc->status=DEVICE_BUSY;
	system.device_desc->serving_request=io_request;
	
	out(0xE0 | (io_request->lba >> 24),0x1F6);
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	out(io_request->command,0x1F7);
	
	for (k = 0;k < 1000;k++);

	//to fix
	if (io_request->command == WRITE_28)
	{
		for (i = 0;i < 256;i++)
		{  
			//out(*(char*)io_request->io_buffer++,0x1F0); 
			outw((unsigned short)57,0x1F0);
		}
	}
	
	//one interrupt for each block
	for (k = 0;k < io_request->sector_count;k++)
	{
		//semaphore to avoid race with interrupt handler
		sem_down(&device_desc->sem);

		if ((in(0x1F7) & 1))
		{
			device_desc->status = DEVICE_IDLE;
			panic();
			return -1;
		}
		if (io_request->command == READ_28)
		{
			for (i = 0;i < 512;i += 2)
			{   
				unsigned short val = inw(0x1F0);
				((char*)io_request->io_buffer)[i+(512*k)] = (val&0xff);
				((char*)io_request->io_buffer)[i+1+(512*k)] = (val>>0x8);
			}
		i=0;
		}
	}
	//Endpoint mutual exclusion region
	sem_up(&device_desc->mutex);	
	return 0;
}

//MODIFIED VERSION TO TEST PERFORMANCE WITH MULTISECTOR READS (SEE _read_test(t_ext2* ext2) in ext2.c)
static unsigned int _read_write_28_ata_____(t_io_request* io_request)
{
	int i;
	t_device_desc* device_desc = NULL;
	t_io_request* pending_request = NULL;
	t_llist_node* node = NULL;
	int k = 0;
	int s;
	
	device_desc = io_request->device_desc;
	//Entrypoint mutual exclusion region
	sem_down(&device_desc->mutex);
	
	device_desc->status = DEVICE_BUSY;
	system.device_desc->serving_request = io_request;
	
        out(2, 0x3F6);
	out(0xE0 | (io_request->lba >> 24),0x1F6);
	//io_request->sector_count=2;
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	//out(io_request->command,0x1F7);
	if (io_request->command == READ_28) 
	{
		out(0xc4,0x1F7);
	}
	for (k = 0;k < 1000; k++);

	//to fix
	if (io_request->command == WRITE_28)
	{
		for (i = 0;i < 256;i++)
		{  
			//out(*(char*)io_request->io_buffer++,0x1F0); 
			outw((unsigned short)57,0x1F0);
		}
	}
	
	//one interrupt for each block
	for (k = 0;k < io_request->sector_count;k++)
	{
		//semaphore to avoid race with interrupt handler
		//sem_down(&device_desc->sem);

		while ((in(0x1F7) & 0x83) != 0);

		if ((in(0x1F7) & 1))
		{
			device_desc->status = DEVICE_IDLE;
			panic();
			return -1;
		}
		if (io_request->command == READ_28)
		{
			for (i = 0;i < 512;i += 2)
			{  
				unsigned short val = inw(0x1F0);
				((char*) io_request->io_buffer)[i + (512 * k)] = (val & 0xff);
				((char*) io_request->io_buffer)[i + 1 + (512 * k)] = (val >> 0x8);
			}
		i = 0;
		}
	}
	//Endpoint mutual exclusion region
	sem_up(&device_desc->mutex);	
	return 0;
}

static unsigned int _p_read_write_28_ata(t_io_request* io_request)
{
	int i;
	t_device_desc* device_desc = NULL;
	t_io_request* pending_request = NULL;
	t_llist_node* node = NULL;
	t_spinlock_desc spinlock;
	int k = 0;

	SPINLOCK_INIT(spinlock);
	device_desc = io_request->device_desc;
	
	//Entrypoint mutual exclusion region.
	SPINLOCK_LOCK(spinlock);
	
	device_desc->status = DEVICE_BUSY || POOLING_MODE;
	system.device_desc->serving_request = io_request;

	out(0x2,0x3F6);
	out(0xE0 | (io_request->lba >> 24),0x1F6);
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	out(io_request->command,0x1F7);
	for (k = 0;k < 1000;k++);

	//to fix
	if (io_request->command == WRITE_28)
	{
		for (i = 0;i < 256;i++)
		{  
			//out(*(char*)io_request->io_buffer++,0x1F0); 
			outw((unsigned short)57,0x1F0);
		}
	}
	while (in(0x1F7) & 0x80);

	if ((in(0x1F7) & 0x21))
	{
		device_desc->status = DEVICE_IDLE;
		panic();
		return -1;
	}
	device_desc->status = DEVICE_IDLE;
	
	if (io_request->command == READ_28)
	{
		for (i = 0;i < (512 * io_request->sector_count);i+=2)
		{  
			unsigned short val = inw(0x1F0);
			((char*) io_request->io_buffer)[i] = (val & 0xff);
			((char*) io_request->io_buffer)[i+1] = (val >> 0x8);
		}
	}
	out(0x0,0x3F6);
	//Exitpoint mutual exclusion region
	SPINLOCK_UNLOCK(spinlock);
	return 0;
}

unsigned int _read_dma_28_ata(t_io_request* io_request)
{
	io_request->command = READ_DMA_28;
	return _read_write_dma_28_ata(io_request);	
}

unsigned int _read_28_ata(t_io_request* io_request)
{
	io_request->command = READ_28;
	return _read_write_28_ata(io_request);	
}

unsigned int _write_28_ata(t_io_request* io_request)
{
	io_request->command = WRITE_28;
	return _read_write_28_ata(io_request);	
}

unsigned int _p_read_28_ata(t_io_request* io_request)
{
	io_request->command = READ_28;
	return _p_read_write_28_ata(io_request);	
}

unsigned int _p_write_28_ata(t_io_request* io_request)
{
	io_request->command = WRITE_28;
	return _p_read_write_28_ata(io_request);	
}

int _flush_ata_pending_request()
{
	int ret = 0;
	if(system.device_desc->serving_request->process_context != NULL)
	{
		_awake(system.device_desc->serving_request->process_context);
		ret = 1;
	}
	return ret;
}
