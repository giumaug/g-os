#ifndef IDT_H                
#define IDT_H

#include "scheduler/process.h"

struct t_i_desc {
   unsigned short int baseLow;    	 
   unsigned short int selector;      	
   unsigned short flags;
   unsigned short baseHi;
};

struct t_idt_ptr {
	unsigned short int idt_size __attribute__((__packed__));
	unsigned int idt_address __attribute__((__packed__));
};

void int_handler_generic();
void init_idt();
void set_idt_entry(int entry,struct t_i_desc* i_desc);
void exit_int_handler(unsigned int action,struct t_processor_reg processor_reg,short ds);

#endif

