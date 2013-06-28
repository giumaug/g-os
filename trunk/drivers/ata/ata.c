#include "asm.h" 
#include "idt.h" 
#include "drivers/ata/ata.h"

void static int_handler_ata(); 

void init_ata(t_ata_desc* ata_desc)
{	
	struct t_i_desc i_desc;
	
	i_desc.baseLow=((int)&int_handler_ata) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x0EF00;
	i_desc.baseHi=((int)&int_handler_ata)>>0x10;
	set_idt_entry(0x2E,&i_desc);
	ata_desc->pending_request=new_dllist();
}

void free_ata()
{
	//nothing to do
}

void int_handler_ata()
{	
	struct t_processor_reg processor_reg;

	SAVE_PROCESSOR_REG
	system.ata_desc->status=REQUEST_COMPLETED;	
	EOI
	_awake(system.ata_desc->serving_process_context);
	RESTORE_PROCESSOR_REG
	RET_FROM_INT_HANDLER
}

unsigned int _read_28_ata(t_ata_desc* ata_desc,unsigned int sector_count,unsigned int lba,void* io_buffer,unsigned int sync)
{
	int i;
	void *io_buffer;
	struct t_process_context* process_context;
	struct t_process_context *current_process_context;	
	
	SAVE_IF_STATUS
	CLI
	current_process_context=system.process_info.current_process->val;
	if (ata_desc->status==REQUEST_WAITING)
	{
		_sleep(processor_reg);
		ll_append(ata_desc->pending_request,current_process_context);
	}
	else 
	{
		ata_desc->status=REQUEST_WAITING;
	}
	
	ata_desc->serving_processs_context=current_process_context;
	out(0xE0 | (ata_request->lba >> 24),0x1F6);
	out(0x00,0x1F1);
	out((unsigned char)ata_request->sector_count,0x1F2);
	out((unsigned char)ata_request->lba,0x1F3);
	out((unsigned char)(ata_request->lba >> 8),0x1F4);
	out((unsigned char)(ata_request->lba >> 16),0x1F5);
	out(READ_28,0x1F7);
	if (!in(0x1F7) & 1)
	{
		return -1;
	}
	if (sync) {
		_sleep();
	}
	else
	{
		while(ata_desc->status!=REQUEST_COMPLETED);
	}
	for (i=0;i<256;i++)
	{  
		//out(*(char*)io_buffer++,0x1F0); 
		int zz=inw(0x1F0);
		((char*)current_ata_request->io_buffer)[i]=zz;
	}

	if (!ll_empty(ata_desc->pending_request))
	{
		process_context=(struct t_process_context*)ll_sentinel(ata_desc->pending_request);
		_awake(process_context);
	}

	RESTORE_IF_STATUS
	return 0;
}

unsigned int _write_28_ata(t_ata_desc* ata_desc,unsigned int sector_count,unsigned int lba,void* io_buffer,unsigned int sync)
{
	int i;
	void *io_buffer;
	struct t_process_context* process_context;
	struct t_process_context *current_process_context;
	
	SAVE_IF_STATUS
	CLI
	current_process_context=system.process_info.current_process->val;
	if (ata_desc->status==REQUEST_WAITING)
	{
		_sleep();
		ll_append(ata_desc->pending_request,current_process_context);
	}	
	else 
	{
		ata_desc->status=REQUEST_WAITING;
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
	if (sync) 
	{
		_sleep();
	}
	else
	{
		while(ata_desc->status!=REQUEST_COMPLETED);
	}
	
	_awake(current_process_context);
	if (!ll_empty(ata_desc->pending_request))
	{
		process_context=(struct t_process_context*)ll_sentinel(ata_desc->pending_request);
		_awake(process_context);
	}
	
	RESTORE_IF_STATUS
	return 0;
}
