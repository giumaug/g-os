#ifndef STDLIB_HEADER
#define STDLIB_HEADER
#ifndef ADD_HEADER
	#include "asm.h"
	#include "syscall_handler.h"
	#include "general.h"
	#define ADD_HEADER
#endif

#define RAND_MAX 4294967294

extern inline int atoi (char *data) __attribute__((always_inline));
extern inline void itoa (int val,char *char_val,unsigned int base) __attribute__((always_inline));
extern inline void exit(int status) __attribute__((always_inline));
extern inline void *malloc(unsigned int mem_size) __attribute__((always_inline));
extern inline void free(void *address) __attribute__((always_inline));
extern inline unsigned int rand(unsigned int *seed) __attribute__((always_inline));

extern struct t_process_info process_info;

extern inline int atoi (char *data)
{
	unsigned int index=0;
	unsigned int sign=1;
	unsigned int digit;
	int val=0;
	char tmp;

	int ret=0;
	if(*data=='-') 
	{
		sign=-1;
		data++;
	}
	val+=*data-48;
	while (*(++data)!='\0') 
	{
		val=val*10+(*data-48);
		tmp++;
	}
	return val;
}

extern inline void itoa (int val,char *char_val,unsigned int base)
{
	unsigned int mod;
	unsigned int res;
	unsigned index=-1;
	unsigned int i;
	char digit;
	char _char_val[32];
	if (val<0) *char_val='-';
	res=val;	
	do
	{
		val=res;
		mod=val % 10;
		res=val/10;
		if (res==0) digit=48+val;
		else digit=48+mod;
		_char_val[++index]=digit;
	}
	while(res!=0);
        for (i=0;i<=index;i++) char_val[i]=_char_val[index-i];
	char_val[++index]='\0';
	//while(_char_val[++index]=='\0') char_val[++index]==_char_val[index];
	return;
}

extern inline void exit(int status)
{
	int params[1];
	params[0]=status;
	SYSCALL(13,params);
	return;
}

extern inline void *malloc(unsigned int mem_size) 
{
	int params[2];
	params[0]=mem_size;
	params[1]=NULL;
	SYSCALL(2,params);
	return params[1];
}

extern inline void free(void *address) 
{	
	int params[1];
	params[0]=address;
	SYSCALL(3,params);
	return;
}

extern inline unsigned int rand(unsigned int *seed)
{
        *seed = *seed * 1103515245 + 12345;
        return (*seed % ((unsigned int)RAND_MAX + 1));
}

//extern inline int rand(void)
//{
//        return (rand_r(&next));
//}
//
//extern inline void srand(unsigned int seed)
//{
//        next = seed;
//}

#endif
