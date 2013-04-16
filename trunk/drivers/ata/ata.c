#include "asm.h" 
#include "idt.h" 
#include "drivers/ata/ata.h"

t_ata_request static *current_ata_request;

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
	if (current_ata_request->cmd==READ_28)
	{
		//Input (E)CX words from port DX into ES:[(E)DI]
		asm("push %ecx");
		asm("push %edx");
		asm("push %edi");
		asm("mov $0x80,%ecx");
		asm("mov $0x1F0,%edx");
		asm ("movl %0,%%edi;"::"r"(current_ata_request->io_buffer));                                  
		asm("rep insw");
		asm("pop %edi");
		asm("pop %edx");
		asm("pop %ecx");
	}
	EOI
	_awake(current_ata_request->process_context);
	RESTORE_PROCESSOR_REG
	RET_FROM_INT_HANDLER
}

void _read_28_ata(t_ata_request *ata_request,struct t_processor_reg* processor_reg)
{
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
	_sleep(processor_reg);
	RESTORE_IF_STATUS
}

void _write_28_ata(t_ata_request *ata_request,struct t_processor_reg* processor_reg)
{
	int i=0;
	int z=0;
	int kk;
	void *io_buffer;	
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
	out(WRITE_28,0x1F7);
	for(i=0;i<=1000000;i++);
	kk=in(0x1F7);
	//(inb(iobase + ATA_STATUS) & ATA_STATUS_ERR) 
	//if (in(0x1F7) & 1)
	{
		z++;
	}
	io_buffer=current_ata_request->io_buffer;
//	asm("push %ecx");
//	asm("push %edx");
//	asm("push %edi");
//	asm("mov $0x80,%ecx");
//	asm("mov $0x1F0,%edx");
//	asm ("movl %0,%%edi;"::"r"(current_ata_request->io_buffer));                                  
//	asm("rep outsw");
//	asm("pop %edi");
//	asm("pop %edx");
//	asm("pop %ecx");

	for (i =0;i<512;i++)
	{  
		out(*(char*)io_buffer++,0x1F0);  
	}
	//_sleep(processor_reg);
	RESTORE_IF_STATUS
}