#include "asm.h" 
#include "idt.h"
#include "scheduler/process.h"
#include "virtual_memory/vm.h"
#include "drivers/pic/8259A.h"
#include "lib/lib.h"
#include "drivers/lapic/lapic.h"

struct t_i_desc idt[255];
struct t_idt_ptr idt_ptr;

void exception()
{
	return;
}

void int_handler_generic() 
{ 
	exception();
	EOI_TO_LAPIC
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_0() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_1() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_2() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_3() 
{ 
	exception(); 
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_4() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_5() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_6() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_7() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_8() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_9() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_10() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_11() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_12() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_13() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_14() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_15() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_16() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_17() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_18() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_19() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_20() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_21() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}





void int_handler_generic_22() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_23() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_24() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_25() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_26() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_27() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_28() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_29() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_30() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_31() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_32() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_33() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_34() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_35() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_36() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_37() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_38() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_39() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_40() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_41() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_42() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_43() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_44() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_45() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_46() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_47() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_48() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_49() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void int_handler_generic_50() 
{
	exception();  
	STI
	asm("pop %ebp;iret");
	return;
}

void init_idt()
{
	int i;
	
//	for (i=0;i<255;i++)
//	{
//		idt[i].baseLow=((int)(&int_handler_generic)) & 0xFFFF;
//		idt[i].selector=0x8;
//		idt[i].flags=0x08e00;
//		idt[i].baseHi=((int)(&int_handler_generic))>>0x010;
//	}


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

	idt[20].baseLow=((int)(&int_handler_generic_20)) & 0xFFFF;
	idt[20].selector=0x8;
	idt[20].flags=0x08e00;
	idt[20].baseHi=((int)(&int_handler_generic_20))>>0x010;

	idt[21].baseLow=((int)(&int_handler_generic_21)) & 0xFFFF;
	idt[21].selector=0x8;
	idt[21].flags=0x08e00;
	idt[21].baseHi=((int)(&int_handler_generic_21))>>0x010;

	idt[22].baseLow=((int)(&int_handler_generic_22)) & 0xFFFF;
	idt[22].selector=0x8;
	idt[22].flags=0x08e00;
	idt[22].baseHi=((int)(&int_handler_generic_22))>>0x010;

	idt[23].baseLow=((int)(&int_handler_generic_23)) & 0xFFFF;
	idt[23].selector=0x8;
	idt[23].flags=0x08e00;
	idt[23].baseHi=((int)(&int_handler_generic_23))>>0x010;

	idt[24].baseLow=((int)(&int_handler_generic_24)) & 0xFFFF;
	idt[24].selector=0x8;
	idt[24].flags=0x08e00;
	idt[24].baseHi=((int)(&int_handler_generic_24))>>0x010;

	idt[25].baseLow=((int)(&int_handler_generic_25)) & 0xFFFF;
	idt[25].selector=0x8;
	idt[25].flags=0x08e00;
	idt[25].baseHi=((int)(&int_handler_generic_25))>>0x010;

	idt[26].baseLow=((int)(&int_handler_generic_26)) & 0xFFFF;
	idt[26].selector=0x8;
	idt[26].flags=0x08e00;
	idt[26].baseHi=((int)(&int_handler_generic_26))>>0x010;

	idt[27].baseLow=((int)(&int_handler_generic_27)) & 0xFFFF;
	idt[27].selector=0x8;
	idt[27].flags=0x08e00;
	idt[27].baseHi=((int)(&int_handler_generic_27))>>0x010;

	idt[28].baseLow=((int)(&int_handler_generic_28)) & 0xFFFF;
	idt[28].selector=0x8;
	idt[28].flags=0x08e00;
	idt[28].baseHi=((int)(&int_handler_generic_28))>>0x010;

	idt[29].baseLow=((int)(&int_handler_generic_29)) & 0xFFFF;
	idt[29].selector=0x8;
	idt[29].flags=0x08e00;
	idt[29].baseHi=((int)(&int_handler_generic_29))>>0x010;

	idt[30].baseLow=((int)(&int_handler_generic_30)) & 0xFFFF;
	idt[30].selector=0x8;
	idt[30].flags=0x08e00;
	idt[30].baseHi=((int)(&int_handler_generic_30))>>0x010;

	idt[31].baseLow=((int)(&int_handler_generic_31)) & 0xFFFF;
	idt[31].selector=0x8;
	idt[31].flags=0x08e00;
	idt[31].baseHi=((int)(&int_handler_generic_31))>>0x010;

	idt[32].baseLow=((int)(&int_handler_generic_32)) & 0xFFFF;
	idt[32].selector=0x8;
	idt[32].flags=0x08e00;
	idt[32].baseHi=((int)(&int_handler_generic_32))>>0x010;

	idt[33].baseLow=((int)(&int_handler_generic_33)) & 0xFFFF;
	idt[33].selector=0x8;
	idt[33].flags=0x08e00;
	idt[33].baseHi=((int)(&int_handler_generic_33))>>0x010;

	idt[34].baseLow=((int)(&int_handler_generic_34)) & 0xFFFF;
	idt[34].selector=0x8;
	idt[34].flags=0x08e00;
	idt[34].baseHi=((int)(&int_handler_generic_34))>>0x010;

	idt[35].baseLow=((int)(&int_handler_generic_35)) & 0xFFFF;
	idt[35].selector=0x8;
	idt[35].flags=0x08e00;
	idt[35].baseHi=((int)(&int_handler_generic_35))>>0x010;

	idt[36].baseLow=((int)(&int_handler_generic_36)) & 0xFFFF;
	idt[36].selector=0x8;
	idt[36].flags=0x08e00;
	idt[36].baseHi=((int)(&int_handler_generic_36))>>0x010;

	idt[37].baseLow=((int)(&int_handler_generic_37)) & 0xFFFF;
	idt[37].selector=0x8;
	idt[37].flags=0x08e00;
	idt[37].baseHi=((int)(&int_handler_generic_37))>>0x010;

	idt[38].baseLow=((int)(&int_handler_generic_38)) & 0xFFFF;
	idt[38].selector=0x8;
	idt[38].flags=0x08e00;
	idt[38].baseHi=((int)(&int_handler_generic_38))>>0x010;

	idt[39].baseLow=((int)(&int_handler_generic_39)) & 0xFFFF;
	idt[39].selector=0x8;
	idt[39].flags=0x08e00;
	idt[39].baseHi=((int)(&int_handler_generic_39))>>0x010;

	idt[40].baseLow=((int)(&int_handler_generic_40)) & 0xFFFF;
	idt[40].selector=0x8;
	idt[40].flags=0x08e00;
	idt[40].baseHi=((int)(&int_handler_generic_40))>>0x010;

	idt[41].baseLow=((int)(&int_handler_generic_41)) & 0xFFFF;
	idt[41].selector=0x8;
	idt[41].flags=0x08e00;
	idt[41].baseHi=((int)(&int_handler_generic_41))>>0x010;

	idt[42].baseLow=((int)(&int_handler_generic_42)) & 0xFFFF;
	idt[42].selector=0x8;
	idt[42].flags=0x08e00;
	idt[42].baseHi=((int)(&int_handler_generic_42))>>0x010;

	idt[43].baseLow=((int)(&int_handler_generic_43)) & 0xFFFF;
	idt[43].selector=0x8;
	idt[43].flags=0x08e00;
	idt[43].baseHi=((int)(&int_handler_generic_43))>>0x010;

	idt[44].baseLow=((int)(&int_handler_generic_44)) & 0xFFFF;
	idt[44].selector=0x8;
	idt[44].flags=0x08e00;
	idt[44].baseHi=((int)(&int_handler_generic_44))>>0x010;

	idt[45].baseLow=((int)(&int_handler_generic_45)) & 0xFFFF;
	idt[45].selector=0x8;
	idt[45].flags=0x08e00;
	idt[45].baseHi=((int)(&int_handler_generic_45))>>0x010;

	idt[46].baseLow=((int)(&int_handler_generic_46)) & 0xFFFF;
	idt[46].selector=0x8;
	idt[46].flags=0x08e00;
	idt[46].baseHi=((int)(&int_handler_generic_46))>>0x010;

	idt[47].baseLow=((int)(&int_handler_generic_47)) & 0xFFFF;
	idt[47].selector=0x8;
	idt[47].flags=0x08e00;
	idt[47].baseHi=((int)(&int_handler_generic_47))>>0x010;

	idt[48].baseLow=((int)(&int_handler_generic_48)) & 0xFFFF;
	idt[48].selector=0x8;
	idt[48].flags=0x08e00;
	idt[48].baseHi=((int)(&int_handler_generic_48))>>0x010;

	idt[49].baseLow=((int)(&int_handler_generic_49)) & 0xFFFF;
	idt[49].selector=0x8;
	idt[49].flags=0x08e00;
	idt[49].baseHi=((int)(&int_handler_generic_49))>>0x010;

	idt[50].baseLow=((int)(&int_handler_generic_50)) & 0xFFFF;
	idt[50].selector=0x8;
	idt[50].flags=0x08e00;
	idt[50].baseHi=((int)(&int_handler_generic_50))>>0x010;
	
	idt_ptr.idt_size=64*256;
        idt_ptr.idt_address=(int)idt;
	asm ("lidt idt_ptr");
}

void set_idt_entry(int entry,struct t_i_desc* i_desc)
{
	idt[entry]=*i_desc;
}

