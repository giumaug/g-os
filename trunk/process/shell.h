#ifndef SHELL_H              
#define SHELL_H

#define LIST_CMD "list"
#define RUN_CMD  "run"
#define PROCESS_NUM 3

typedef struct s_user_process 
{
	char name[10];
	unsigned int id;
	unsigned int size;
	unsigned int start_addr;
}
t_user_process;

t_user_process user_process[PROCESS_NUM]=
{
	{"process1",1,0x100000,0x600000},
	{"process2",2,0x100000,0x700000},
	{"process3",3,0x100000,0x800000}
};

#endif



