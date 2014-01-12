#include "asm.h" 
#include "idt.h"
#include "general.h"
#include "virtual_memory/vm.h"
#include "drivers/ata/ata.h"

extern int test;
extern t_system system;

void static int_handler_ata();

void init_ata(t_device_desc* device_desc)
{	
	struct t_i_desc i_desc;
	
	i_desc.baseLow=((int)&int_handler_ata) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x0EF00;
	i_desc.baseHi=((int)&int_handler_ata)>>0x10;
	set_idt_entry(0x2E,&i_desc);
	device_desc->read=_read_28_ata;
	device_desc->write=_write_28_ata;
	device_desc->status=DEVICE_IDLE;
	sem_init(device_desc->sem);
}

void free_ata(t_device_desc* device_desc)
{
	free_llist(device_desc->pending_request);
}

void int_handler_ata()
{	
	struct t_processor_reg processor_reg;
	t_io_request* io_request;

//	CLI
	SAVE_PROCESSOR_REG
	disable_irq_line(14);
	system.int_path_count++;
	EOI_TO_SLAVE_PIC
	EOI_TO_MASTER_PIC

	if (system.device_desc->serving_request->process_context!=NULL)
	{
		_awake(system.device_desc->serving_request->process_context);
	}
	enable_irq_line(14);
	EXIT_INT_HANDLER(0,processor_reg,0)
}

static unsigned int _read_write_28_ata(t_io_request* io_request)
{
	int i;
	struct t_process_context* process_context;
	struct t_process_context *current_process_context;
	t_device_desc* device_desc;
	t_io_request* pending_request;
	t_llist_node* node;
	
	device_desc=io_request->device_desc;
	sem_down(device_desc->sem);
	device_desc->status=DEVICE_BUSY;
	
	out(0xE0 | (io_request->lba >> 24),0x1F6);
	out((unsigned char)io_request->sector_count,0x1F2);
	out((unsigned char)io_request->lba,0x1F3);
	out((unsigned char)(io_request->lba >> 8),0x1F4);
	out((unsigned char)(io_request->lba >> 16),0x1F5);
	out(io_request->command,0x1F7);

	if (io_request->command==WRITE_28)
	{
		for (i=0;i<256;i++)
		{  
			//out(*(char*)io_request->io_buffer++,0x1F0); 
			outw((unsigned short)57,0x1F0);
		}
	}
	system.device_desc->serving_request=io_request;
	if (system.process_info.current_process->val!=NULL)
	{
		io_request->process_context=system.process_info.current_process->val;
		_sleep();
	}
	else
	{
		while(device_desc->status==DEVICE_BUSY);
	}

	if ((in(0x1F7)&1))
	{
		device_desc->status=DEVICE_IDLE;
		panic();
		return -1;
	}
	
	if (io_request->command==READ_28)
	{
		for (i=0;i<256;i++)
		{  
			//out(*(char*)io_buffer++,0x1F0); 
			int zz=inw(0x1F0);
			((char*)io_request->io_buffer)[i]=zz;
		}
	}
	device_desc->status=DEVICE_IDLE;
	sem_up(device_desc->sem);
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
