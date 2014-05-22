#include "debug.h" 
#include "asm.h" 
#include "idt.h"
#include "general.h"
#include "virtual_memory/vm.h"
#include "drivers/ata/ata.h"

static int race=0;
int test=0;
extern t_system system;

void static int_handler_ata();

void init_ata(t_device_desc* device_desc)
{	
	struct t_i_desc i_desc;
	
	i_desc.baseLow=((int)&int_handler_ata) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x08e00;
	i_desc.baseHi=((int)&int_handler_ata)>>0x10;
	set_idt_entry(0x2E,&i_desc);
	device_desc->read=_read_28_ata;
	device_desc->write=_write_28_ata;
	device_desc->p_read=_p_read_28_ata;
	device_desc->p_write=_p_write_28_ata;
	device_desc->status=DEVICE_IDLE;
	sem_init(&device_desc->mutex,1);
	sem_init(&device_desc->sem,0);
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

	SAVE_PROCESSOR_REG
	disable_irq_line(14);
	DISABLE_PREEMPTION
	EOI_TO_SLAVE_PIC
	EOI_TO_MASTER_PIC
	STI

	io_request=system.device_desc->serving_request;
	process_context=io_request->process_context;
	if (!system.device_desc->status==POOLING_MODE)
	{
		sem_up(&io_request->device_desc->sem);
	}
	system.device_desc->status=DEVICE_IDLE;
	enable_irq_line(14);
	ENABLE_PREEMPTION
	EXIT_INT_HANDLER(0,processor_reg,0)
}

static unsigned int _read_write_28_ata(t_io_request* io_request)
{
	int i;
	t_device_desc* device_desc;
	t_io_request* pending_request;
	t_llist_node* node;
	int k=0;
	
	device_desc=io_request->device_desc;
	//Entrypoint mutual exclusion region
	sem_down(&device_desc->mutex);
	race++;
	
	device_desc->status=DEVICE_BUSY;
	system.device_desc->serving_request=io_request;
	
	out(0xE0 | (io_request->lba >> 24),0x1F6);
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	out(io_request->command,0x1F7);
	for (k=0;k<1000;k++);

	//to fix
	if (io_request->command==WRITE_28)
	{
		for (i=0;i<256;i++)
		{  
			//out(*(char*)io_request->io_buffer++,0x1F0); 
			outw((unsigned short)57,0x1F0);
		}
	}
	
	//semaphore to avoid race with interrupt handler
	sem_down(&device_desc->sem);

	if ((in(0x1F7)&1))
	{
		device_desc->status=DEVICE_IDLE;
		panic();
		return -1;
	}
	
	if (io_request->command==READ_28)
	{
		for (i=0;i<(512*io_request->sector_count);i+=2)
		{  
			unsigned short val=inw(0x1F0);
			((char*)io_request->io_buffer)[i]=(val&0xff);
			((char*)io_request->io_buffer)[i+1]=(val>>0x8);
		}
	}
	race--;
	if (race>0)
	{
		panic();
	}
	//Endpoint mutual exclusion region
	sem_up(&device_desc->mutex);	
	return 0;
}

static unsigned int _p_read_write_28_ata(t_io_request* io_request)
{
	int i;
	t_device_desc* device_desc;
	t_io_request* pending_request;
	t_llist_node* node;
	t_spinlock_desc spinlock;
	int k=0;

	SPINLOCK_INIT(spinlock);
	device_desc=io_request->device_desc;
	
	//Entrypoint mutual exclusion region
	SPINLOCK_LOCK(spinlock);
	
	device_desc->status=DEVICE_BUSY || POOLING_MODE;
	system.device_desc->serving_request=io_request;

	out(0xE0 | (io_request->lba >> 24),0x1F6);
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	out(io_request->command,0x1F7);
	for (k=0;k<10000000;k++);

	//to fix
	if (io_request->command==WRITE_28)
	{
		for (i=0;i<256;i++)
		{  
			//out(*(char*)io_request->io_buffer++,0x1F0); 
			outw((unsigned short)57,0x1F0);
		}
	}
	while (in(0x1F7)&0x80);

	if ((in(0x1F7)&0x21))
	{
		device_desc->status=DEVICE_IDLE;
		panic();
		return -1;
	}

	device_desc->status=DEVICE_IDLE;
	
	if (io_request->command==READ_28)
	{
		for (i=0;i<(512*io_request->sector_count);i+=2)
		{  
			unsigned short val=inw(0x1F0);
			((char*)io_request->io_buffer)[i]=(val&0xff);
			((char*)io_request->io_buffer)[i+1]=(val>>0x8);
		}
	}
	//Exitpoint mutual exclusion region
	SPINLOCK_UNLOCK(spinlock);
	return 0;
}

unsigned int _read_28_ata(t_io_request* io_request)
{
	io_request->command=READ_28;
	return _read_write_28_ata(io_request);	
}

unsigned int _write_28_ata(t_io_request* io_request)
{
	io_request->command=WRITE_28;
	return _read_write_28_ata(io_request);	
}

unsigned int _p_read_28_ata(t_io_request* io_request)
{
	io_request->command=READ_28;
	return _p_read_write_28_ata(io_request);	
}

unsigned int _p_write_28_ata(t_io_request* io_request)
{
	io_request->command=WRITE_28;
	return _p_read_write_28_ata(io_request);	
}



int _flush_ata_pending_request()
{
	int ret=0;
	if (system.device_desc->serving_request->process_context!=NULL)
	{
		_awake(system.device_desc->serving_request->process_context);
		ret=1;
	}
	return ret;
}
