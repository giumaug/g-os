#include "lib.h"

void static print_num(unsigned int val)
{
	unsigned int mod;
	unsigned int res;
	unsigned index=-1;
	unsigned int i;
	char digit;
	char _char_val[32];
	char char_val[32];
	int params[1];
	
	itoa (val,char_val,10);	
	while (char_val[++index]!='\0') 
	{
		params[0]=char_val[index];
		SYSCALL(4,params);
	}
}

void static print_char(char* text)
{
	int index=-1;
	int params[1];
	while (text[++index]!='\0')
	{
		params[0]=text[index];
		SYSCALL(4,params);
	}
}

void printf(char* text,...)
{
	int index=-1;
	int param_index=0;
	unsigned int** param_val;
	int params[1];
	int sys_num=4;

	while (text[++index]!='\0')
	{
		if (text[index]=='%' && text[index+1]=='d')
		{
			index+=2;
			param_index++;
			GET_FROM_STACK(param_index,param_val);
			print_num(*param_val);

		}
		else if (text[index]=='%' && text[index+1]=='s')
		{
			index+=2;
			param_index++;
			GET_FROM_STACK(param_index,param_val);
			print_char(*param_val);

		}
		params[0]=text[index];
		SYSCALL(4,params);
	}
} 

void scanf(char *text,void *val)
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

int getc()
{
	char data;
	int params[1];

	SYSCALL(7,NULL);
	
	params[0]=&data;
	SYSCALL(5,params);
	
//	params[0]=data;
//	SYSCALL(6,params);
	
	if (data=='\b') 
	{
		SYSCALL(10,NULL);
		SYSCALL(9,NULL);
	}
	else if (data!='\r')
	{
		params[0]=data;
		SYSCALL(6,params);
	}

	SYSCALL(8,NULL);
	return data;
}

int remove(const char *filename)
{
	unsigned int params[2];

	params[0]=filename;
	SYSCALL(22,params);
	return  params[1];
}
