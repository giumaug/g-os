#include "common.h"
#include "asm.h" 
#include "idt.h"
#include "virtual_memory/vm.h"
#include "pci/pci.h"
#include "drivers/ata/ata.h"

void static int_handler_ata();

/*
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
*/

t_device_desc* init_ata(u8 device_num)
{
	t_device_desc* device_desc = NULL;
	struct t_i_desc i_desc;
	u32 bar4 = NULL;
	u32 pci_command = NULL;
	struct t_process_context* current_process_context = NULL;
	u8 int_entry;
	
	//device_desc = kmalloc(sizeof(t_device_desc));
	//device_desc->num = device_num;
	device_desc = init_device(device_num, 2);
	i_desc.baseLow = ((int)&int_handler_ata) & 0xFFFF;
	i_desc.selector = 0x8;
	i_desc.flags = 0x08e00;
	i_desc.baseHi = ((int)&int_handler_ata)>>0x10;
	set_idt_entry(0x2E,&i_desc);

	device_desc->read_dma = _read_dma_28_ata;
	device_desc->write_dma = _write_dma_28_ata;
	device_desc->read = _read_28_ata;
	device_desc->write = _write_28_ata;
	device_desc->p_read = _p_read_28_ata;
	device_desc->p_write = _p_write_28_ata;
	device_desc->status = DEVICE_IDLE;
	//sem_init(&device_desc->mutex,1);
	//sem_init(&device_desc->sem,0);

	bar4 = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_BAR4);
	//BUS MASTER BIT SET-UP (2 bit starting from 0)
	pci_command = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND);
    pci_command |= 0x4;
	write_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND,pci_command);
	//pci_command = read_pci_config_word(ATA_PCI_BUS,ATA_PCI_SLOT,ATA_PCI_FUNC,ATA_PCI_COMMAND);

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
	return device_desc;
}

void free_ata(t_device_desc* device_desc)
{
	device_desc->status=DEVICE_IDLE;
	sem_down(&device_desc->mutex);
	kfree(device_desc);
}

void int_handler_ata()
{
	struct t_processor_reg processor_reg;
	t_io_request* io_request = NULL;
	struct t_process_context* process_context = NULL;
	struct t_process_context* current_process_context = NULL;

	SAVE_PROCESSOR_REG
	disable_irq_line(14);
	DISABLE_PREEMPTION
	EOI_TO_SLAVE_PIC
	EOI_TO_MASTER_PIC
	STI
	CURRENT_PROCESS_CONTEXT(current_process_context);
	
	io_request = system.device_desc->serving_request;
	process_context = io_request->process_context;

	if (system.device_desc->status == DEVICE_BUSY)  //POOLING_MODE
	{
		sem_up(&system.device_desc->sem);
	}
	if (current_process_context->pid != process_context->pid) 
	{
	 	system.force_scheduling = 1;
	}
	//system.device_desc->status = DEVICE_IDLE;
	enable_irq_line(14);
	ENABLE_PREEMPTION
	//--EXIT_INT_HANDLER(0,processor_reg)

	static struct t_process_context _current_process_context;                                                  	
	static struct t_process_context _old_process_context;                                                      	
	static struct t_process_context _new_process_context;	                                                        
	static struct t_processor_reg _processor_reg;                                                                   
	static unsigned int _action2; 
	static u8 stop = 0;                                                                             
                                                                                                                    
	CLI                                                                                                             
	if (system.int_path_count == 0 && system.force_scheduling == 0 && system.flush_network == 1)                    
	{                                                                                                               
		system.flush_network = 0;                                                                       
		dequeue_packet(system.network_desc);                                                            
		equeue_packet(system.network_desc);                                                             
		system.flush_network = 1;                                                                       
	}                                                                                                               
	_action2=0;                                                                                      
	_current_process_context=*(struct t_process_context*)system.process_info->current_process->val;                 
	_old_process_context=_current_process_context;                                                                  
	_processor_reg=processor_reg;                                                                                   
	if (system.force_scheduling == 1 && 0 == 0 && system.int_path_count == 0)                                  
	{                                                                                                               
		_action2 = 1;                                                                                           
		if (_current_process_context.proc_status == EXITING)                                                    
		{                                                                                                       
			_action2 = 2;                                                                                   
		}                                                                                                       
	}                                                                                                               
                                                                                                                        
	if (_action2>0)                                                                                                 
	{	system.force_scheduling = 0;
		stop = 0;                                                                            
		while(!stop)                                                                                             
		{                                                                                                       
			schedule(&_current_process_context,&_processor_reg);                                            
			_new_process_context = *(struct t_process_context*) system.process_info->current_process->val;  
			if (_new_process_context.sig_num == SIGINT)                                                    
			{                                                                                      
				_exit(0);
				free_vm_process(&_new_process_context);                                                         
				buddy_free_page(system.buddy_desc,FROM_PHY_TO_VIRT(_new_process_context.phy_kernel_stack));           
			}                                                                                               
			else                                                                                            
			{                                                                                               
				stop = 1;                                                                               
			}                                                                                               
		}      
		if (_new_process_context.pid != _old_process_context.pid)                                               
		{                                                                                                       
				_processor_reg=_new_process_context.processor_reg;                                      
		}                                                                                                       
		SWITCH_PAGE_DIR(FROM_VIRT_TO_PHY(((unsigned int) _new_process_context.page_dir)))                       
		DO_STACK_FRAME(_processor_reg.esp-8);                                                                   
                                                                                                                        
		if (_action2==2)                                                                                        
		{                                                                                                       
			DO_STACK_FRAME(_processor_reg.esp-8);                                                           
			free_vm_process(&_old_process_context);                                                         
			buddy_free_page(system.buddy_desc,FROM_PHY_TO_VIRT(_old_process_context.phy_kernel_stack));     
		}                                                                                                       
		RESTORE_PROCESSOR_REG                                                                                   
		EXIT_SYSCALL_HANDLER                                                                                    
	}                                                                                                          	
	else                                                                                                       	
	{                                                                                                               
		RESTORE_PROCESSOR_REG                                                                                   
		RET_FROM_INT_HANDLER                                                                                    
	}
}

static unsigned int _read_write_dma_28_ata(t_io_request* io_request)
{
	int i;
	t_device_desc* device_desc = NULL;
	t_io_request* pending_request = NULL;
	t_llist_node* node = NULL;
	char* prd = NULL;
	char* prd_aligned = NULL;
	short byte_count;
	int ret = 0;
	u8 dma_status;
	u16 cmd_status;
	
	device_desc = io_request->device_desc;
	//Entrypoint mutual exclusion region.
	//Here all requestes are enqueued using semaphore internal queue.
	//In order to implement a multilevel disk scheduler a better design
	//is to use an external queue with multilevel priority slots.
	sem_down(&device_desc->mutex);
	device_desc->status=DEVICE_BUSY;
	device_desc->serving_request=io_request;
	prd = kmalloc(16);
	prd_aligned = ((((u32)prd % 0x10) != 0) ? ((((u32)prd + 0x10) - (((u32)prd + 0x10) % 0x10))) : ((u32)prd % 0x10));  
	byte_count = io_request->sector_count * SECTOR_SIZE;
	prd_aligned[0] = LOW_OCT_32((u32)io_request->io_buffer);
	prd_aligned[1] = MID_RGT_OCT_32((u32)io_request->io_buffer);
	prd_aligned[2] = MID_LFT_OCT_32((u32)io_request->io_buffer); 
	prd_aligned[3] = HI_OCT_32((u32)io_request->io_buffer);
	prd_aligned[4] = LOW_16(byte_count);
	prd_aligned[5] = HI_16(byte_count); 
	prd_aligned[6] = 0;
	prd_aligned[7] = 0x80;
		
	write_config_dword(device_desc,ATA_DMA_PRD_REG,FROM_VIRT_TO_PHY(prd_aligned));
	
	out(0xE0 | (device_desc->num * 16) | (unsigned char)(io_request->lba >> 24),0x1F6);
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	out(io_request->command,0x1F7);

	write_config_byte(device_desc,ATA_DMA_COMMAND_REG,0x1);
	//semaphore to avoid race with interrupt handler
	sem_down(&device_desc->sem);
	write_config_byte(device_desc,ATA_DMA_COMMAND_REG,0x0);	
	dma_status = read_config_byte(device_desc,ATA_DMA_STATUS_REG);
	cmd_status = in(0x1F7);
	if ((cmd_status & 1) || dma_status & 1)
	{
		panic();//only for debugger
		ret = -1;
	}
	device_desc->status=DEVICE_IDLE;
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

//	if (io_request->command == 0x30 && io_request->lba == 2052)
//	{
//		//panic();
//	}

	device_desc = io_request->device_desc;
	//Entrypoint mutual exclusion region.
	//Here all requestes are enqueued using semaphore internal queue.
	//In order to implement a multilevel disk scheduler a better design
	//is to use an external queue with multilevel priority slots.
	sem_down(&device_desc->mutex);
	device_desc->status=DEVICE_BUSY;
	device_desc->serving_request=io_request;
	out(0xE0 | (device_desc->num * 16) | (unsigned char)(io_request->lba >> 24),0x1F6);
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	out(io_request->command,0x1F7);
	
	for (k = 0;k < 1000;k++);

	//one interrupt for each block
	for (k = 0;k < io_request->sector_count;k++)
	{
		//semaphore to avoid race with interrupt handler
		//sem_down(&device_desc->sem);
		if (io_request->command == WRITE_28)
		{
			for (i = 0;i < 512;i += 2)
			{    
				u8 low_val = ((char*)io_request->io_buffer)[i+(512*k)];
				u8 hi_val = ((char*)io_request->io_buffer)[i+1+(512*k)];
				u16 val = low_val + (hi_val << 0x8);
				outw((unsigned short)val,0x1F0);
			}
			sem_down(&device_desc->sem);
			int yyy = in(0x1F7);
			if (yyy == 1)
			{
				printk("...\n");
			}
			if ((in(0x1F7) & 1))
			{
				device_desc->status = DEVICE_IDLE;
				panic();
				return -1;
			}
		}
		if (io_request->command == READ_28)
		{
			//semaphore to avoid race with interrupt handler
			sem_down(&device_desc->sem);
			if ((in(0x1F7) & 1))
			{
				device_desc->status = DEVICE_IDLE;
				panic();
				return -1;
			}
			for (i = 0;i < 512;i += 2)
			{   
				unsigned short val = inw(0x1F0);
				((char*)io_request->io_buffer)[i+(512*k)] = (val&0xff);
				((char*)io_request->io_buffer)[i+1+(512*k)] = (val>>0x8);
			}
			i=0;
		}
	}
	device_desc->status=DEVICE_IDLE;
	//Endpoint mutual exclusion region
	sem_up(&device_desc->mutex);	
	return 0;
}


//COMPLETELY BROKEN!!!!! IT WORKS BECAUSE USED AT STARTUP ONLY AS READ!!!!!!!!!!!!!!!!!!!!!!!!!..
static unsigned int _p_read_write_28_ata(t_io_request* io_request)
{
	int xxx;

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
	device_desc->serving_request = io_request;
	out(0x2,0x3F6);
	out(0xE0 | (device_desc->num * 16) | (io_request->lba >> 24),0x1F6);
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	out(io_request->command,0x1F7);
	for (k = 0;k < 1000;k++);

	if (io_request->command == WRITE_28)
	{	
		for (i = 0;i < 512;i += 2)
		{    
			u8 low_val = ((char*)io_request->io_buffer)[i+(512*k)];
			u8 hi_val = ((char*)io_request->io_buffer)[i+1+(512*k)];
			u16 val = low_val + (hi_val << 0x8);
			outw((unsigned short)val,0x1F0);	
		}
		while (in(0x1F7) & 0x80);
		if ((in(0x1F7) & 0x21))
		{
			device_desc->status = DEVICE_IDLE;
			panic();
			return -1;
		}
		device_desc->status = DEVICE_IDLE;
	}
	if (io_request->command == READ_28)
	{
		while (in(0x1F7) & 0x80);
		if ((in(0x1F7) & 0x21))
		{  
			xxx = in(0x1F7); 
			device_desc->status = DEVICE_IDLE;
			panic();
			return -1;
		}
		device_desc->status = DEVICE_IDLE;
		for (i = 0;i < (512 * io_request->sector_count);i+=2)
		{  
			unsigned short val = inw(0x1F0);
			((char*) io_request->io_buffer)[i] = (val & 0xff);
			((char*) io_request->io_buffer)[i+1] = (val >> 0x8);
		}
	}
	out(0x0,0x3F6);
    device_desc->status=DEVICE_IDLE;
	//Exitpoint mutual exclusion region
	SPINLOCK_UNLOCK(spinlock);
	return 0;
}

unsigned int _write_dma_28_ata(t_io_request* io_request)
{
	//SECOND
	io_request->command = WRITE_DMA_28;
	return _read_write_dma_28_ata(io_request);	
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
	//FIRST
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

void _select_dev(int device_num)
{
	system.device_desc->num = device_num;
}
