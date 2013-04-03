#include "asm.h" 
#include "drivers/ata/ata.h" 

void init_ata()
{	
	struct t_i_desc i_desc;
	
	in_buf=new_queue();
	i_desc.baseLow=((int)&int_handler_ata) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x0EF00;
	i_desc.baseHi=((int)&int_handler_kbc)>>0x10;
	set_idt_entry(0x2E,&i_desc);
}

void free_ata()
{

}

void int_handler_ata()
{

}

void read_28_ata(unsigned int lba)
{
//Send 0xE0 for the "master" or 0xF0 for the "slave", ORed with the highest 4 bits of the LBA to port 0x1F6: outb(0x1F6, 0xE0 | (slavebit << 4) | ((LBA >> 24) & 0x0F))
//Send a NULL byte to port 0x1F1, if you like (it is ignored and wastes lots of CPU time): outb(0x1F1, 0x00)
//Send the sectorcount to port 0x1F2: outb(0x1F2, (unsigned char) count)
//Send the low 8 bits of the LBA to port 0x1F3: outb(0x1F3, (unsigned char) LBA))
//Send the next 8 bits of the LBA to port 0x1F4: outb(0x1F4, (unsigned char)(LBA >> 8))
//Send the next 8 bits of the LBA to port 0x1F5: outb(0x1F5, (unsigned char)(LBA >> 16))
//Send the "READ SECTORS" command (0x20) to port 0x1F7: outb(0x1F7, 0x20) 
	out(0x1F6, 0xE0 | (LBA >> 24))

}

void write_28_ata()
{

}



