#ifndef I8254X_H                
#define I8254X_H

#include "system.h"

#define I8254X_BUS 		XX
#define I8254X_SLOT 		XX
#define I8254X_FUNC 		XX
#define I8254X_MEM_BASE 	XX
#define I8254X_IRQ_LINE 	XX
#define RX_BUF_SIZE         	0x2000
#define TX_BUF_SIZE


#define CTRL_REG	 	0x0000
#define CTRL_SLU 		0x40

#define MLTC_TBL_ARRY_REG       0x5200
#define TDBAL_REG               0x03800 			//Transmit Descriptor Base Low
#define TDBAH_REG               0x03804 			//Transmit Descriptor Base High
#define TDLEN_REG               0x03808 			//Transmit Descriptor Length
#define THD_REG			0x3810  			//Transmit Descriptor Head
#define TDT_REG			0x3818  			//Transmit Descriptor Tail

#define REG_TCTRL		0x400				//Trasmit control register
#define TCTL_EN                 (1 << 1)    			//Transmit Enable
#define TCTL_PSP         	(1 << 3)    			//Pad Short Packets
#define TCTL_CT_SHIFT          	0x4           			//Collision Threshold
#define TCTL_COLD_SHIFT         0xc          			//Collision Distance
#define TCTL_RTLC               (1 << 24)   			//Re-transmit on Late Collision


#define RDBAL_REG       	0x2800  			//Receive Descriptor Base Low
#define RDBAH_REG  		0x2804  			//Receive Descriptor Base High
#define RDLEN_REG       	0x2808  			//Receive Descriptor Length
#define RHD_REG			0x2810  			//Receive Descriptor Head
#define RDT_REG			0x2818  			//Receive Descriptor Tail

#define RCTRL_REG		0x100 				//receive control register
#define	RCTL_EN 		(1 << 1)    			//Receiver Enable
#define	RCTL_SBP		(1 << 2)    			//Store Bad Packets
#define	RCTL_UPE       		(1 << 3)    			//Unicast Promiscuous Enabled
#define	RCTL_MPE               	(1 << 4)    			//Multicast Promiscuous Enabled
#define	RCTL_LBM_NONE		(0 << 6)    			//No Loopback
#define	RTCL_RDMTS_HALF         (0 << 8)    			//Free Buffer Threshold is 1/2 of RDLEN
#define	RCTL_BAM               	(1 << 15)   			//Broadcast Accept Mode
#define	RCTL_SECRC             	(1 << 26)   			//Strip Ethernet CRC
#define	RCTL_BSIZE_8192        	((2 << 16) | (1 << 25))

#define NUM_RX_DESC             0x20   				//Receive payload buffer size
#define NUM_TX_DESC             0x20   				//Transmit payload buffer size

#define TSTA_DD                 (1 << 0)    			//Descriptor Done
#define REG_EERD		0x14				//EEPROM Read 

#define REG_ICR			0xC0 				//Interrupt Cause Read 
#define ICR_LSC			0x4				//Link Status Change
#define ICR_RXDMT0 		0x10				//Receive Descriptor Minimum Threshold Reached
#define ICR_RXT0 		0x80				//Receiver Timer Interrupt




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