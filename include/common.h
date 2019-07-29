#ifndef COMMON_H                
#define COMMON_H

#define LOW_32(data)			(data & 0xFFFF)
#define HI_32(data)			((data>>16) & 0xFFFF)
#define LOW_16(data) 			(data & 0xFF)
#define HI_16(data)  			((data>>8) & 0xFF)
#define SWAP_WORD(data)           	((data>>8) & 0xFF) | ((data)<<8 & 0xFFFF)
#define GET_WORD(a,b)			(b | ((a<<8) & 0xFFFF ))
#define GET_DWORD(a,b,c,d)		(d | ((c<<8) & 0xFFFF) | ((b<<16) & 0xFFFFFF ) | (a<<24))
#define LOW_OCT_32(data)		(data & 0xFF)
#define MID_RGT_OCT_32(data)		((data>>8) & 0xFF)
#define MID_LFT_OCT_32(data)		((data>>16) & 0xFF)
#define HI_OCT_32(data)			((data>>24) & 0xFF)

#define SET_BIT(value,bit)             	value |= 1 << bit;
#define CLEAR_BIT(value,bit)            value &= ~(1 << bit);

#define ALIGN_TO_BOUNDARY(boundary,address)                                                                                  	\
					(((address % boundary) != 0) ?                                                        	\
					(((address + boundary) - ((address + boundary) % boundary))) : (address))  

#endif
