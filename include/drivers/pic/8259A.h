#ifndef M8254A_H                
#define M8254A_H

#include "system.h"

#define ICW_1 	   	0x11			 //00010001 binary. Enables initialization mode and we are sending ICW 4
#define PIC_1_CTRL    	0x20			 //Primary PIC control register
#define PIC_2_CTRL 	0xA0			 //Secondary PIC control register
#define PIC_1_DATA 	0x21		         //Primary PIC data register
#define PIC_2_DATA 	0xA1		         //Secondary PIC data register
#define IRQ_0		0x20		         //IRQs 0-7 mapped to use interrupts 0x20-0x27
#define IRQ_8		0x28		         //IRQs 8-15 mapped to use interrupts 0x28-0x36

void init_pic();
void enable_irq_line(int numirq);
void disable_irq_line(int numirq);

#endif
