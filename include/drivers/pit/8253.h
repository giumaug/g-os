#ifndef M8253_H                
#define M8253_H

#define COUNTER_0 		0x40
#define COUNTER_1 		0x41
#define COUNTER_2 		0x42
#define CONTROL_WORD		0x43
#define BINARIY_COUNT_MODE 	0x36 //0B110110 
#define QUANTUM_DURATION	10 //10 ms
#define QUANTUM			11931 //10ms
#define HI_QUANTUM		QUANTUM >> 8
#define LOW_QUANTUM		QUANTUM & 0xff


void int_handler_pit();

#endif


