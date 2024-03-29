#include "asm.h"
#include "lib/lib.h"

static void itoa(unsigned int val,char *char_val,unsigned int base)
{
	unsigned int mod;
	unsigned int res;
	unsigned index=-1;
	unsigned int i;
	char digit;
	char _char_val[11]; 
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
	return;
}

void static printk_num(int val)
{
	char char_val[11]; //int32
	itoa (val,char_val,10);
	printk(char_val);
}

void static printk_char(char* text)
{
	int index=-1;
	struct t_process_context *current_process_context=system.process_info->current_process->val;
	t_console_desc *console_desc=current_process_context->console_desc;
	
	while (text[++index]!='\0')
	{
		_write_char_no_irq(console_desc,text[index]);
	}
}

void printk(char *text,...)
{
	int index=-1;
	int param_index=0;
	long long **param_val;
	int params[2];
	struct t_process_context *current_process_context=system.process_info->current_process->val;
	t_console_desc *console_desc=current_process_context->console_desc;

	u32 xxx=&text;

	while (text[++index]!='\0')
	{
		if (text[index]=='%' && text[index+1]=='d')
		{
			index+=2;
			param_index++;
			GET_FROM_STACK(param_index,param_val);
			printk_num(*param_val);
		}
		if (text[index]=='%' && text[index+1]=='s')
		{
			index+=2;
			param_index++;
			GET_FROM_STACK(param_index,param_val);
			printk_char(*param_val);
		}

		_write_char_no_irq(console_desc,text[index]);
	}
}
