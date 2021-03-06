#ifndef STDIO_HEADER
#define STDIO_HEADER
#ifndef ADD_HEADER
	#include "asm.h"
	#include "syscall_handler.h"
	#include "general.h"
	#define ADD_HEADER
#endif
#include "lib/stdlib.h"

//extern inline void printf(char *text,...) __attribute__((always_inline));
extern inline void printf(char *text) __attribute__((always_inline));
extern inline void d_printf(int val) __attribute__((always_inline));
extern inline void scanf(char *text,void* val) __attribute__((always_inline));
extern inline void print_num(int val) __attribute__((always_inline));
extern inline int remove(const char *filename) __attribute__((always_inline));

extern struct t_process_info process_info;

extern inline void print_num(int val)
{
	unsigned int mod;
	unsigned int res;
	unsigned index=-1;
	unsigned int i;
	char digit;
	char _char_val[32];
	char char_val[32]; //int32
	int params[1];
	
	itoa (val,char_val,10);	
	while (char_val[++index]!='\0') 
	{
		params[0]=char_val[index];
		SYSCALL(4,params);
	}
}

extern inline void printf(char *text)
{
	int index=-1;
	int params[1];
	while (text[++index]!='\0')
	{
		params[0]=text[index];
		SYSCALL(4,params);
	}
}

extern inline void d_printf(int val)
{
	print_num(val);
}

//CAN'T INLINE VARIADIC FUNCTION!!!!!!!!!!!!!
/*extern inline void printf(char *text,...)
{
	int index=-1;
	int param_index=0;
	int** param_val;
	int params[1];
	int sys_num=4;

	while (text[++index]!='\0')
	{
		if (text[index]=='%' && text[index+1]=='d')
		{
			index+=2;
			param_index++;
			GET_FROM_STACK(param_index,param_val);
			//print_num(**param_val);

		}
		params[0]=text[index];
		SYSCALL(4,params);
	}
}*/ 

extern inline void scanf(char *text,void *val)
{
	char data;
	char char_val[100];
	int params[2];
	int index=0;
	unsigned int i=0;

	SYSCALL(7,NULL);
	do
	{
		params[0]=&data;
		SYSCALL(5,params);
		if (data!='\r') 
		{
			if (data=='\b' && index!=0) 
			{
				SYSCALL(10,NULL);
				SYSCALL(9,NULL);
				index--;
			}
			else if (data!='\b') 
			{
				char_val[index++]=data;
				params[0]=data;
				SYSCALL(6,params);
			}
		}
	}
	while (data!='\r');
	SYSCALL(8,NULL);
	char_val[index]='\0';
	if (text[0]=='%' && text[1]=='d')
	{
		*(int*)val=atoi (char_val);
	}
	else 
	{
		for (i=0;i<=index;i++) 
		{
			((char*)val)[i]=char_val[i];
		}
	}
}

extern inline int remove(const char *filename)
{
	unsigned int params[2];

	params[0]=filename;
	SYSCALL(22,params);
	return  params[1];
}

#endif
