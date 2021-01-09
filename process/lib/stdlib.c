#include "lib.h"

#define RAND_MAX 4294967294

unsigned int atoi (unsigned char *data)
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

void itoa (unsigned int val,char *char_val,unsigned int base)
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

void exit(int status)
{
	int params[1];
	params[0]=status;
	SYSCALL(13,params);
	return;
}

void* malloc(unsigned int mem_size) 
{
	return _malloc(mem_size);
}

void free(void *address)
{	
	_free(address);
	return;
}

unsigned int rand()
{
		static unsigned int seed = SEED;
        seed = seed * 1103515245 + 12345;
        return (seed % ((unsigned int)RAND_MAX + 1));
}
