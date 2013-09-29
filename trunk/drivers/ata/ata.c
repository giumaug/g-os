#include "asm.h" 
#include "idt.h"
#include "general.h"
#include "drivers/ata/ata.h"

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
	device_desc->pending_request=new_dllist();
	device_desc->read=_read_28_ata;
	device_desc->write=_write_28_ata;
}

void free_ata(t_device_desc* device_desc)
{
	free_llist(device_desc->pending_request);
}

void int_handler_ata()
{	
	struct t_processor_reg processor_reg;

	SAVE_PROCESSOR_REG
	system.device_desc->status=REQUEST_COMPLETED;	
	EOI
	_awake(system.device_desc->serving_process_context);
	EXIT_INT_HANDLER(0,processor_reg,0)
}

unsigned int _read_28_ata(t_device_desc* device_desc,unsigned int sector_count,unsigned int lba,void* io_buffer)
{
	int i;
	struct t_process_context* process_context;
	struct t_process_context *current_process_context;	
	
	SAVE_IF_STATUS
	CLI
	current_process_context=system.process_info.current_process->val;
	if (device_desc->status==REQUEST_WAITING)
	{
		_sleep();
		ll_append(device_desc->pending_request,current_process_context);
	}
	else 
	{
		device_desc->status=REQUEST_WAITING;
	}
	out(0xE0 | (lba >> 24),0x1F6);
	out(0x00,0x1F1);
	out((unsigned char)sector_count,0x1F2);
	out((unsigned char)lba,0x1F3);
	out((unsigned char)(lba >> 8),0x1F4);
	out((unsigned char)(lba >> 16),0x1F5);
	out(READ_28,0x1F7);
	if (!in(0x1F7) & 1)
	{
		return -1;
	}
	if (system.process_info.current_process->val!=NULL)
	{
		_sleep();
	}
	else
	{
		while(device_desc->status!=REQUEST_COMPLETED);
	}
	for (i=0;i<256;i++)
	{  
		//out(*(char*)io_buffer++,0x1F0); 
		int zz=inw(0x1F0);
		((char*)io_buffer)[i]=zz;
	}

	if (!ll_empty(device_desc->pending_request))
	{
		process_context=(struct t_process_context*)ll_sentinel(device_desc->pending_request);
		_awake(process_context);
	}

	RESTORE_IF_STATUS
	return 0;
}

unsigned int _write_28_ata(t_device_desc* device_desc,unsigned int sector_count,unsigned int lba,void* io_buffer)
{
	int i;
	struct t_process_context* process_context;
	struct t_process_context *current_process_context;
	
	SAVE_IF_STATUS
	CLI
	current_process_context=system.process_info.current_process->val;
	if (device_desc->status==REQUEST_WAITING)
	{
		_sleep();
		ll_append(device_desc->pending_request,current_process_context);
	}	
	else 
	{
		device_desc->status=REQUEST_WAITING;
	}
	
	out(0xE0 | (lba >> 24),0x1F6);
	out((unsigned char)sector_count,0x1F2);
	out((unsigned char)lba,0x1F3);
	out((unsigned char)(lba >> 8),0x1F4);
	out((unsigned char)(lba >> 16),0x1F5);
	out(WRITE_28,0x1F7);

	if (!in(0x1F7) & 1)
	{
		return -1;
	}

	for (i=0;i<256;i++)
	{  
		//out(*(char*)io_buffer++,0x1F0); 
		outw((unsigned short)55,0x1F0);
	}
	if (system.process_info.current_process->val!=NULL)
	{
		_sleep();
	}
	else
	{
		while(device_desc->status!=REQUEST_COMPLETED);
	}
	
	_awake(current_process_context);
	if (!ll_empty(device_desc->pending_request))
	{
		process_context=(struct t_process_context*)ll_sentinel(device_desc->pending_request);
		_awake(process_context);
	}
	
	RESTORE_IF_STATUS
	return 0;
}
