#include "lib/stdio.h" 
#include "lib/stdlib.h"
#include "system.h"

extern t_system system;

extern unsigned int exit_count_1;
extern unsigned int exit_count_2;
extern unsigned int exit_count_3;
extern unsigned int exit_count_4;
extern unsigned int exit_count_5;
extern unsigned int exit_count_6;

process_2()
{
	unsigned int read_params[3];
	unsigned int write_params[3];
	unsigned int y,z;
	unsigned char* write_mem_addr;
	unsigned char* read_mem_addr;
	unsigned char* mem_addr;
	unsigned int workload;

	exit_count_5++;
	workload=(rand() % 20 + 1);

	unsigned int pid=((struct t_process_context*)system.process_info.current_process->val)->pid;
	printf("\nrequest workload=");
        d_printf(workload);
	printf("\n");
	printf("\npid=");
	d_printf(pid);
	printf("\n");
//	workload=2;
	for (y=0;y<workload;y++)
	{
		write_mem_addr=malloc(512);
		for (z=0;z<512;z++)
		{
			*(write_mem_addr+z)=0x55;
		}
		write_params[0]=1;
		write_params[1]=10;
		write_params[2]=write_mem_addr;
		SYSCALL(25,write_params);
		free(write_mem_addr);
		printf(".");
		d_printf(pid);
	}

//	for (z=0;z<512;z++)
//	{
//		*(write_mem_addr+z)=0xFF;
//	}

//	write_mem_addr=malloc(512);
//	read_mem_addr=malloc(512);

//	write_params[0]=1;
//	write_params[1]=10;
//	write_params[2]=write_mem_addr;
//	SYSCALL(25,write_params);

//	read_params[0]=1;
//	read_params[1]=10;
//	read_params[2]=read_mem_addr;
//	SYSCALL(24,read_params);

//	free(write_mem_addr);
//	free(read_mem_addr);
	exit(0);
}
