#include "asm.h" 
#include "idt.h" 
#include "drivers/ata/ata.h"

t_ata_request static *current_ata_request;
static unsigned int status;

void static int_handler_ata(); 

void init_ata()
{	
	struct t_i_desc i_desc;
	
	i_desc.baseLow=((int)&int_handler_ata) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x0EF00;
	i_desc.baseHi=((int)&int_handler_ata)>>0x10;
	set_idt_entry(0x2E,&i_desc);
}

void free_ata()
{

}

void int_handler_ata()
{	
	struct t_processor_reg processor_reg;

	SAVE_PROCESSOR_REG
	current_ata_request->status=REQUEST_COMPLETED;	
	EOI
	_awake(current_ata_request->process_context);
	RESTORE_PROCESSOR_REG
	RET_FROM_INT_HANDLER
}

unsigned int _read_28_ata(t_ata_request *ata_request,unsigned int sync)
{
	int i;	
	SAVE_IF_STATUS
	CLI
	//sleep process if dirver locked by other request
	current_ata_request=ata_request;	
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
	if (!sync) {
		_sleep(ata_request->processor_reg);
	}
	else
	{
		while(current_ata_request->status!=REQUEST_COMPLETED);
	}
	for (i=0;i<256;i++)
	{  
		//out(*(char*)io_buffer++,0x1F0); 
		int zz=inw(0x1F0);
		((char*)current_ata_request->io_buffer)[i]=zz;
	}
	RESTORE_IF_STATUS
	return 0;
}

//unsigned int _write_28_ata(t_ata_request *ata_request,unsigned int sync)
unsigned int _write_28_ata(unsigned int sector_count,
			   unsigned int lba,void* io_buffer,
			   struct t_processor_reg* processor_reg,
			   struct t_process_context *current_process_context)
{
	int i;
	void *io_buffer;	
	SAVE_IF_STATUS
	CLI
	//sleep process if dirver locked by other request

	if (status==0)
	{
		sleep();
	}	

	current_ata_request=ata_request;	
	out(0xE0 | (ata_request->lba >> 24),0x1F6);
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
	if (!sync) 
	{
		_sleep(processor_reg);
	}
	else
	{
		while(status!=REQUEST_COMPLETED);
	}
	//check waiting queue
	RESTORE_IF_STATUS
	return 0;
}
