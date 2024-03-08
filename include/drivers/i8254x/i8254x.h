#ifndef I8254X_H                
#define I8254X_H

#include "system.h"
#include "network/network.h"
#include "asm.h" 
#include "idt.h"
#include "virtual_memory/vm.h"
#include "pci/pci.h"
#include "data_types/primitive_types.h"
#include "network/common.h"

#define I8254X_VIRT_BAR0_MEM    	0x80000
#define I8254X_VIRT_BAR0_MEM_SIZE 	0x10000

#define I8254X_BUS 				    0x0 						   //82567LF-2
#define I8254X_SLOT 				0x19
#define I8254X_FUNC 				0x0

//#define I8254X_BUS 				0x1 					       //i82543gc
//#define I8254X_SLOT 				0x0
//#define I8254X_FUNC 				0x0

#define I8254X_BAR0	 				0x10
#define I8254X_BAR1 				0x14
#define I8254X_IRQ_LINE 			0x3F
#define RX_BUF_SIZE         		0x2000
#define TX_BUF_SIZE

#define CTRL_REG	 				0x0000
#define STATUS_REG              	0x0008
#define TDFPC_REG					0x3430
#define TDFH_REG      				0x3410 						//hw head
#define TDFT_REG	 				0x3418 						//hw tail

#define CTRL_SLU 					0x40

#define MLTC_TBL_ARRY_REG       	0x5200
#define TDBAL_REG               	0x03800 					//Transmit Descriptor Base Low
#define TDBAH_REG               	0x03804 					//Transmit Descriptor Base High
#define TDLEN_REG               	0x03808 					//Transmit Descriptor Length
#define THD_REG						0x3810  					//Transmit Descriptor Head
#define TDT_REG						0x3818  					//Transmit Descriptor Tail

#define TCTRL_REG					0x400						//Trasmit control register
#define TPG_REG                     0x410                       //Trasmit IPG (82567LF-2)
#define TCTL_EN                 	(1 << 1)    				//Transmit Enable
#define TCTL_PSP         			(1 << 3)    				//Pad Short Packets
#define TCTL_CT_SHIFT          		0x4           				//Collision Threshold
#define TCTL_COLD_SHIFT         	0xc          				//Collision Distance
#define TCTL_RTLC               	(1 << 24)   				//Re-transmit on Late Collision

#define RDBAL_REG       			0x2800  					//Receive Descriptor Base Low
#define RDBAH_REG  					0x2804  					//Receive Descriptor Base High
#define RDLEN_REG       			0x2808  					//Receive Descriptor Length
#define RHD_REG						0x2810  					//Receive Descriptor Head
#define RDT_REG						0x2818  					//Receive Descriptor Tail

#define RCTRL_REG					0x100 						//receive control register
#define	RCTL_EN 					(1 << 1)    				//Receiver Enable
#define	RCTL_SBP					(1 << 2)    				//Store Bad Packets
#define	RCTL_UPE       				(1 << 3)    				//Unicast Promiscuous Enabled
#define	RCTL_MPE               		(1 << 4)    				//Multicast Promiscuous Enabled
#define	RCTL_LBM_NONE				(0 << 6)    				//No Loopback
#define	RTCL_RDMTS_HALF         	(0 << 8)    				//Free Buffer Threshold is 1/2 of RDLEN
#define	RCTL_BAM               		(1 << 15)   				//Broadcast Accept Mode
#define	RCTL_SECRC             		(1 << 26)   				//Strip Ethernet CRC
#define	RCTL_BSIZE_8192        		((2 << 16) | (1 << 25))

#define NUM_RX_DESC_BIT         	0x10
#define NUM_RX_DESC             	0x10 						//Receive payload buffer size
#define NUM_TX_DESC             	0x10   						//Transmit payload buffer size

#define TSTA_DD                 	(1 << 0)    				//Descriptor Done
#define REG_EERD					0x14						//EEPROM Read 

#define REG_ICR						0xC0 						//Interrupt Cause Read 
#define ICR_LSC						0x4							//Link Status Change
#define ICR_RXDMT0 					0x10						//Receive Descriptor Minimum Threshold Reached
#define ICR_RXT0 					0x80						//Receiver Timer Interrupt
#define ICR_RXO 					0x40                        //Receiver Overrun Interrupt

#define REG_IMS						0xD0						//Interrupt Mask Set/Read Register
#define IMS_RXT0	        		0x080
#define IMS_RXO                 	0x40

#define CMD_EOP	   					(1<<0)		
#define CMD_RS     					(1<<3)
#define CMD_RPS    					(1<<4)
#define CMD_IFCS   					(1<<1)

typedef struct __attribute__((packed)) s_rx_desc_i8254x 
{
	volatile u32 low_addr;
    volatile u32 hi_addr;
    volatile u16 length;
    volatile u16 checksum;
    volatile u8 status;
    volatile u8 errors;
    volatile u16 special;
}
t_rx_desc_i8254x;
 
typedef struct __attribute__((packed)) s_tx_desc_i8254x 
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

typedef struct s_i8254x
{
	u8 bar_type;
	u32* mem_base;
	u32 mem_base_size;
	u16 io_base;
	u8 irq_line;
	u32 low_mac;
	u32 hi_mac;
	t_rx_desc_i8254x* rx_desc;
	t_tx_desc_i8254x* tx_desc;
	u16 rx_cur;
	u16 tx_cur;
	t_mac_addr mac_addr;
}
t_i8254x;

t_i8254x* init_8254x();
void free_8254x(t_i8254x* i8254x);
void int_handler_i8254x();
void send_packet_i8254x(t_i8254x* i8254x,void* frame_addr,u16 frame_len);
void testx();

#endif
