#ifndef IOAPIC_H                
#define IOAPIC_H

#include "system.h"

#define IOREGSEL 0xFEC00000
#define IOREGWIN 0xFEC00010
#define IOREDTBL 0x2

//See IOAPIC.PDF par 3.2.4
#define REDTBL_LOW_ENTRY 0x0020
#define REDTBL_HI_ENTRY 0x0000

void init_ioapic();
void mask_entry(u8 offset);
void unmask_entry(u8 offset);

#endif
