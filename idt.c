
#include "asm.h"
#include "idt.h"
#include "drivers/pic/8259A.h"
#include "klib/printk.h"

struct t_i_desc idt[255];
struct t_idt_ptr idt_ptr;

void int_handler_generic() 
{  
	//printk("inside generic int handler \n");
	//while(1);
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}
    
void init_idt()
{
	int i;
	
	for (i=0;i<255;i++)
	{
		idt[i].baseLow=((int)(&int_handler_generic)) & 0xFFFF;
		idt[i].selector=0x8;
		idt[i].flags=0x08e00;
		idt[i].baseHi=((int)(&int_handler_generic))>>0x010;
	}
	idt_ptr.idt_size=64*256;
        idt_ptr.idt_address=(int)idt;
	asm ("lidt idt_ptr");
	//asm ("lidt (%0)": :"p" (idt_ptr));
	//asm ("lidt (%0)": :"r"(idt_ptr));
}

void set_idt_entry(int entry,struct t_i_desc* i_desc)
{
	idt[entry]=*i_desc;
}
