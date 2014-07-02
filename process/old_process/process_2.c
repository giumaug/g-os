#include "lib/stdio.h" 
#include "lib/stdlib.h"
#include "system.h"

extern t_system system;

process_2()
{
	unsigned int read_params[3];
	unsigned int write_params[3];
	unsigned int y,z,i;
	unsigned char* write_mem_addr;
	unsigned char* read_mem_addr;
	unsigned char* mem_addr;
	unsigned int workload;

	workload=(rand() % 100 + 1);

	unsigned int pid=((struct t_process_context*)system.process_info.current_process->val)->pid;
	printf("\nrequest workload=");
        d_printf(workload);
	printf("\n");
	printf("\n--pid=");
	d_printf(pid);
	printf("\n");
	workload=5;
//	workload=10;
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
	exit(0);
}
