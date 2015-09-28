#ifndef I8254X_H                
#define I8254X_H

#include "system.h"

#define I8254X_BUS 		XX
#define I8254X_SLOT 		XX
#define I8254X_FUNC 		XX
#define I8254X_MEM_BASE 	XX
#define I8254X_IRQ_LINE 	XX
#define CTRL	 		0x0000
#define SLU 			0x40
#define MLTC_TBL_ARRY 		0x5200
#define TDBAL                   0x03800
#define TDBAH                   0x03804
#define TDLEN                   0x03808
#define NUM_RX_DESC             0x20
#define NUM_TX_DESC             0x20
#define RX_BUF_SIZE             0x2000
#define TX_BUF_SIZE

typedef struct s_i8254x
{
	u32 mem_base;
	u8 irq_line;
	u32 low_mac;
	u32 hi_mac:
}
t_i8254x;

typedef struct __attribute__((packed)) s_rx_desc_i8254x 
{
	volatile u32 low_addr:
        volatile u32 hi_addr;
        volatile u16 length;
        volatile u16 checksum;
        volatile u8 status;
        volatile u8 errors;
        volatile u16 special;
}
t_rx_desc_i8254x;
 
typedef struct __attribute__((packed)) tx_desc_i8254x 
{
        volatile u32 low_addr;
	volatile u32 hi_addr;
        volatile u16 length;
        volatile u8 cso;
        volatile u8 cmd;
        volatile u8 status;
        volatile u8 css;
        volatile u16 special;
}
t_tx_desc_i8254x;

#endif
