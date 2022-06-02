#ifndef M8253_H                
#define M8253_H

#include "system.h"

#define COUNTER_0 				0x40
#define COUNTER_1 				0x41
#define COUNTER_2 				0x42
#define CONTROL_WORD			0x43
#define BINARIY_COUNT_MODE 		0x36 //0B110110 
#define QUANTUM_DURATION		10 //10 ms
#define QUANTUM					11931 //10ms
#define HI_QUANTUM				QUANTUM >> 8
#define LOW_QUANTUM				QUANTUM & 0xff

//#define STAND_BY_QUANTUM		11931 //1000ms
//#define HI_STAND_BY_QUANTUM		QUANTUM >> 8
//#define LOW_STAND_BY_QUANTUM	QUANTUM & 0xff

void init_pit();
void int_handler_pit();
void free_pit();

#endif


