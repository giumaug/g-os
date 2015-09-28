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
#define TDBAL

typedef struct s_i8254x
{
	u32 mem_base;
	u8 irq_line;
	u32 low_mac;
	u32 hi_mac:
}
t_i8254x;


#endif
