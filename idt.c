
#include "asm.h"
#include "idt.h"
#include "drivers/pic/8259A.h"
#include "klib/printk.h"

struct t_i_desc idt[255];
struct t_idt_ptr idt_ptr;

void exception()
{
	return;
}

void int_handler_generic() 
{  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_0() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_1() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_2() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_3() 
{ 
	exception(); 
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_4() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_5() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_6() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_7() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_8() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_9() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_10() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_11() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_12() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_13() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_14() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_15() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_16() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_17() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_18() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_19() 
{
	exception();  
	EOI
	STI
	asm("pop %ebp;iret");
	return;
}
 
void init_idt()
{
	int i;

	idt[0].baseLow=((int)(&int_handler_generic_0)) & 0xFFFF;
	idt[0].selector=0x8;
	idt[0].flags=0x08e00;
	idt[0].baseHi=((int)(&int_handler_generic_0))>>0x010;

	idt[1].baseLow=((int)(&int_handler_generic_1)) & 0xFFFF;
	idt[1].selector=0x8;
	idt[1].flags=0x08e00;
	idt[1].baseHi=((int)(&int_handler_generic_1))>>0x010;

	idt[2].baseLow=((int)(&int_handler_generic_2)) & 0xFFFF;
	idt[2].selector=0x8;
	idt[2].flags=0x08e00;
	idt[2].baseHi=((int)(&int_handler_generic_2))>>0x010;

	idt[3].baseLow=((int)(&int_handler_generic_3)) & 0xFFFF;
	idt[3].selector=0x8;
	idt[3].flags=0x08e00;
	idt[3].baseHi=((int)(&int_handler_generic_3))>>0x010;

	idt[4].baseLow=((int)(&int_handler_generic_4)) & 0xFFFF;
	idt[4].selector=0x8;
	idt[4].flags=0x08e00;
	idt[4].baseHi=((int)(&int_handler_generic_4))>>0x010;

	idt[5].baseLow=((int)(&int_handler_generic_5)) & 0xFFFF;
	idt[5].selector=0x8;
	idt[5].flags=0x08e00;
	idt[5].baseHi=((int)(&int_handler_generic_5))>>0x010;

	idt[6].baseLow=((int)(&int_handler_generic_6)) & 0xFFFF;
	idt[6].selector=0x8;
	idt[6].flags=0x08e00;
	idt[6].baseHi=((int)(&int_handler_generic_6))>>0x010;

	idt[7].baseLow=((int)(&int_handler_generic_7)) & 0xFFFF;
	idt[7].selector=0x8;
	idt[7].flags=0x08e00;
	idt[7].baseHi=((int)(&int_handler_generic_7))>>0x010;

	idt[8].baseLow=((int)(&int_handler_generic_8)) & 0xFFFF;
	idt[8].selector=0x8;
	idt[8].flags=0x08e00;
	idt[8].baseHi=((int)(&int_handler_generic_8))>>0x010;

	idt[9].baseLow=((int)(&int_handler_generic_9)) & 0xFFFF;
	idt[9].selector=0x8;
	idt[9].flags=0x08e00;
	idt[9].baseHi=((int)(&int_handler_generic_9))>>0x010;

	idt[10].baseLow=((int)(&int_handler_generic_10)) & 0xFFFF;
	idt[10].selector=0x8;
	idt[10].flags=0x08e00;
	idt[10].baseHi=((int)(&int_handler_generic_10))>>0x010;

	idt[11].baseLow=((int)(&int_handler_generic_11)) & 0xFFFF;
	idt[11].selector=0x8;
	idt[11].flags=0x08e00;
	idt[11].baseHi=((int)(&int_handler_generic_11))>>0x010;

	idt[12].baseLow=((int)(&int_handler_generic_12)) & 0xFFFF;
	idt[12].selector=0x8;
	idt[12].flags=0x08e00;
	idt[12].baseHi=((int)(&int_handler_generic_12))>>0x010;

	idt[13].baseLow=((int)(&int_handler_generic_13)) & 0xFFFF;
	idt[13].selector=0x8;
	idt[13].flags=0x08e00;
	idt[13].baseHi=((int)(&int_handler_generic_13))>>0x010;

	idt[14].baseLow=((int)(&int_handler_generic_14)) & 0xFFFF;
	idt[14].selector=0x8;
	idt[14].flags=0x08e00;
	idt[14].baseHi=((int)(&int_handler_generic_14))>>0x010;

	idt[15].baseLow=((int)(&int_handler_generic_15)) & 0xFFFF;
	idt[15].selector=0x8;
	idt[15].flags=0x08e00;
	idt[15].baseHi=((int)(&int_handler_generic_15))>>0x010;

	idt[16].baseLow=((int)(&int_handler_generic_16)) & 0xFFFF;
	idt[16].selector=0x8;
	idt[16].flags=0x08e00;
	idt[16].baseHi=((int)(&int_handler_generic_16))>>0x010;

	idt[17].baseLow=((int)(&int_handler_generic_17)) & 0xFFFF;
	idt[17].selector=0x8;
	idt[17].flags=0x08e00;
	idt[17].baseHi=((int)(&int_handler_generic_17))>>0x010;

	idt[18].baseLow=((int)(&int_handler_generic_18)) & 0xFFFF;
	idt[18].selector=0x8;
	idt[18].flags=0x08e00;
	idt[18].baseHi=((int)(&int_handler_generic_18))>>0x010;

	idt[19].baseLow=((int)(&int_handler_generic_19)) & 0xFFFF;
	idt[19].selector=0x8;
	idt[19].flags=0x08e00;
	idt[19].baseHi=((int)(&int_handler_generic_19))>>0x010;
	
	for (i=0;i<19;i++)
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
