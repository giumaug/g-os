#ifndef PRIMITIVE_TYPES_H                
#define PRIMITIVE_TYPES_H

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

//GENERIC SIZE UNSIGNED INT
#define U_N_INT(n) typedef struct  { unsigned int  val: n ;}

#endif
