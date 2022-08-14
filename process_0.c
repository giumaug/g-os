#include "asm.h"
#include "lib/lib.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "process_0.h"

//_exec SWITCHES KERNEL THREAD TO USERSPACE PROCESS
// THREAD_FORK CLONES KERNEL THREAD
// THREAD_EXEC CREATES NEW KERNEL THREAD TO BE IMPLEMENTED!!!!!!!!!
void process_0() 
{
	unsigned int pid;
	unsigned int child_pid;
	static char* argv[2];
	static char argv1[] = "/shell";	
	
	argv[0] = argv1;
	argv[1] = NULL;	

//    THREAD_FORK(pid);
//	if (pid == 0)
//	{
//		child_pid = _getpid();
//		_setpgid(child_pid,child_pid);
//		_tcsetpgrp(child_pid);
//		_exec("/shell",argv);	
//	}
//	else 
//	{
//		THREAD_EXIT(0);
//	}
	fake_shell();
	THREAD_EXIT(0);
}

t_console_desc* console_desc = NULL;

void fake_shell()
{
	int i;
	int scan_code = 0;
	struct t_process_context* current_process_context = NULL;
	char cmd[300];
	
	CURRENT_PROCESS_CONTEXT(current_process_context);
	console_desc = current_process_context->console_desc;
	
	_printf("G-SHELL V 0.1 \n");
/*
	_printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
	_printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa11111111111111111111\n");
	_printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa22222222222222222222\n");
	_printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa33333333333333333333\n");
	_printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa44444444444444444444\n");
	_printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa55555555555555555555\n");
	_printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa66666666666666666666\n");
	_printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa77777777777777777777\n");
	_printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa88888888888888888888\n");
	_printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa99999999999999999999\n");
	_printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb11111111111111111111\n");
	_printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb22222222222222222222\n");
	_printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb33333333333333333333\n");
	_printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb44444444444444444444\n");
	_printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb55555555555555555555\n");
	_printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb66666666666666666666\n");
	_printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb77777777777777777777\n");
	_printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb88888888888888888888\n");
	_printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb99999999999999999999\n");
	_printf("ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc11111111111111111111\n");
	_printf("ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc22222222222222222222\n");
	_printf("ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc33333333333333333333\n");
	_printf("ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc44444444444444444444\n");
	_printf("ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc55555555555555555555\n");
	_printf("ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc66666666666666666666\n");
	_printf("ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc77777777777777777777\n");
	_printf("ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc88888888888888888888\n");
	_printf("ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc99999999999999999999\n");
	_printf("ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd11111111111111111111\n");
	_printf("ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd22222222222222222222\n");
	_printf("ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd33333333333333333333\n");
*/

	_printf("aaaaaaaaaaa111111111111111111 \n");
	_printf("aaaaaaaaaaa222222222222222222 \n");
	_printf("aaaaaaaaaaa333333333333333333 \n");
	_printf("aaaaaaaaaaa444444444444444444 \n");
	_printf("aaaaaaaaaaa555555555555555555 \n");
	_printf("aaaaaaaaaaa666666666666666666 \n");
	_printf("aaaaaaaaaaa777777777777777777 \n");
	_printf("aaaaaaaaaaa888888888888888888 \n");
	_printf("aaaaaaaaaaa999999999999999999 \n");
	_printf("aaaaaaaaaaa000000000000000000 \n");
	_printf("bbbbbbbbbbb111111111111111111 \n");
	_printf("bbbbbbbbbbb222222222222222222 \n");
	_printf("bbbbbbbbbbb333333333333333333 \n");
	_printf("bbbbbbbbbbb444444444444444444 \n");
	_printf("bbbbbbbbbbb555555555555555555 \n");
	_printf("bbbbbbbbbbb666666666666666666 \n");
	_printf("bbbbbbbbbbb777777777777777777 \n");
	_printf("bbbbbbbbbbb888888888888888888 \n");
	_printf("bbbbbbbbbbb999999999999999999 \n");
	_printf("bbbbbbbbbbb000000000000000000 \n");
	_printf("ccccccccccc111111111111111111 \n");
	_printf("ccccccccccc222222222222222222 \n");
	_printf("ccccccccccc333333333333333333 \n");
	_printf("ccccccccccc444444444444444444 \n");
	_printf("ccccccccccc555555555555555555 \n");
	_printf("ccccccccccc666666666666666666 \n");
	_printf("ccccccccccc777777777777777777 \n");
	_printf("ccccccccccc888888888888888888 \n");
	_printf("ccccccccccc999999999999999999 \n");

	while(1)
	{
		_printf("=>");
		_scanf("%s", &cmd);
		_printf("\necho command: %s \n", cmd);
	}
}

static unsigned int _atoi (unsigned char *data)
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

static void _itoa (unsigned int val,char *char_val,unsigned int base)
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

void static _print_num(unsigned int val)
{
	unsigned int mod;
	unsigned int res;
	unsigned index=-1;
	unsigned int i;
	char digit;
	char _char_val[32];
	char char_val[32];
	int params[1];
	
	_itoa (val,char_val,10);	
	while (char_val[++index]!='\0') 
	{
		_write_char(console_desc, char_val[index]);
	}
}

void static _print_char(char* text)
{
	int index=-1;
	int params[1];
	while (text[++index]!='\0')
	{
		_write_char(console_desc, text[index]);
	}
}

void _printf(char* text,...)
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
			_print_num(*param_val);

		}
		else if (text[index]=='%' && text[index+1]=='s')
		{
			index+=2;
			param_index++;
			GET_FROM_STACK(param_index,param_val);
			_print_char(*param_val);

		}
		_write_char(console_desc, text[index]);
	}
} 

void _scanf(char *text,void *val)
{
	char data;
	char char_val[500];
	int params[2];
	int index=0;
	unsigned int i=0;

	_enable_cursor(console_desc);
	do
	{
		data = _read_char(console_desc);
		if (data!='\r') 
		{
			if (data=='\b' && index!=0) 
			{
				_delete_char(console_desc);
				_update_cursor(console_desc);
				index--;
			}
			else if (data!='\b') 
			{
				char_val[index++]=data;
				_echo_char(console_desc, data);
			}
		}
	}
	while (data!='\r');
	_disable_cursor(console_desc);
	char_val[index]='\0';
	if (text[0]=='%' && text[1]=='d')
	{
		*(int*)val= _atoi (char_val);
	}
	else 
	{
		for (i=0;i<=index;i++) 
		{
			((char*)val)[i]=char_val[i];
		}
	}
}

int _getc()
{
	char data;
	int params[1];

	_enable_cursor(console_desc);
	
	data = _read_char(console_desc);
	
	if (data=='\b') 
	{
		_delete_char(console_desc);
		_update_cursor(console_desc);
	}
	else if (data!='\r')
	{
		_echo_char(console_desc, data);
	}

	_disable_cursor(console_desc);
	return data;
}
