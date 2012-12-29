#include "asm.h"  
#include "drivers/pic/8259A.h" 

void init_pic()
{
	//Send ICW 1 - Begin initialization
	out(ICW_1,PIC_1_CTRL);
	out(ICW_1,PIC_2_CTRL);
	
	//send ICW 2 to primary PIC
	out(IRQ_0,PIC_1_DATA);
	out(IRQ_8,PIC_2_DATA);

	//Send ICW 3 - Set the IR line to connect both PICs 
	out(0x04,PIC_1_DATA);
	out(0x02,PIC_2_DATA);

	//Send ICW 4 - Set x86 mode
	out(0x01,PIC_1_DATA);	
	out(0x01,PIC_2_DATA);

	//All done. Null out the data registers
	out(0x0,PIC_1_DATA);
	out(0x0,PIC_2_DATA);
}

void enable_irq_line(int numirq)  
{  
     if (numirq < 8) {  
         out(in(PIC_1_DATA) & ~(1 << numirq),PIC_1_DATA);  
     } else {    
         out(in(PIC_2_DATA) & ~(1 << (numirq - 8)),PIC_2_DATA);  
     }  
}  
   
void disable_irq_line(int numirq)  
{  
     if (numirq < 8) {    
         out(in(PIC_1_DATA) | (1 << numirq),PIC_1_DATA);  
     } else {    
         out(in(PIC_2_DATA) | (1 << (numirq - 8)),PIC_2_DATA);  
     }  
}  



