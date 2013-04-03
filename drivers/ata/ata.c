#include "drivers/ata/ata.h" 

void init_ata()
{	
	struct t_i_desc i_desc;
	
	//printk("kbc init \n");
	in_buf=new_queue();
	i_desc.baseLow=((int)&int_handler_kbc) & 0xFFFF;
	i_desc.selector=0x8;
	i_desc.flags=0x0EF00;//0x08e00;
	i_desc.baseHi=((int)&int_handler_kbc)>>0x10;
	set_idt_entry(0x21,&i_desc);
}

void free_ata()
{
	kfree(in_buf);
}

void int_handler_ata()
{

}

void read_28_ata()
{

}

void write_28_ata()
{

}



