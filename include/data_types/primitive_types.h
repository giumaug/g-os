#ifndef PRIMITIVE_TYPES_H                
#define PRIMITIVE_TYPES_H

#define NULL 0
#define NULL 0
#define EOF -1
#define TRUE 1
#define FALSE 0

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef int s32;
typedef short s16;
typedef char s8;

//GENERIC SIZE UNSIGNED INT
#define U_N_INT(n) typedef struct  { unsigned int  val: n ;}

#endif
