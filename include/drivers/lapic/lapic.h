#ifndef LAPIC_H                
#define LAPIC_H

#include "system.h"

#define LAPIC_BASE 0xfee00000
#define LAPIC_ID 0x20
#define LAPIC_DFR  0x00e0
#define LAPIC_LDR  0x00d0
#define LAPIC_TPR  0x0080
#define LAPIC_SVR  0x00f0
#define LAPIC_DCR 0x03e0     
#define LAPIC_TMR 0x0320
#define LAPIC_TMR_ONESHOT 0x0
#define LAPIC_TMR_M (1 << 16)
#define LAPIC_TMR_PERIODIC (1 << 17)
#define LAPIC_TMR_VECTOR 0x38
#define EOI 0x00b0

#define LAPIC_ICR 0x0380
#define LAPIC_CCR 0x0390

#define LAPIC_SVR_APIC_EN 0x100
#define LAPIC_LVT_DV_1 0xb
#define LAPIC_LVT_DV_2 0x0
#define LAPIC_LVT_DV_4 0x1     
#define LAPIC_LVT_DV_8 0x2
#define LAPIC_LVT_DV_16 0x3
#define LAPIC_LVT_DV_32 0x8
#define LAPIC_LVT_DV_64 0x9
#define LAPIC_LVT_DV_128 0xa

#define ONE_SEC_DELAY 100
#define TICK_FRQ 100
#define EOI_TO_LAPIC unsigned char* p = (LAPIC_BASE + EOI); \
    *p = 0;

void init_lapic();

#endif
