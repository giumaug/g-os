#include "lib/stdio.h" 
#include "lib/stdlib.h"
#include "system.h"

#define SEED 212430

extern t_system system;

process_2()
{
	unsigned int read_params[3];
	unsigned int write_params[3];
	unsigned int y,z;
	unsigned char* write_mem_addr;
	unsigned char* read_mem_addr;
	unsigned char* mem_addr;
	unsigned int workload;
	unsigned int seed=SEED;

//	workload=(_rand(&seed) % 20 + 1);
//
//	for (y=0;y<workload;y++)
//	{
//
//		mem_addr=malloc(512);
//		for (z=0;z<512;z++)
//		{
//			*(mem_addr+z)=0xFF;
//		}
//		free(mem_addr);
//	}

	for (z=0;z<512;z++)
	{
		*(write_mem_addr+z)=0xFF;
	}

	write_mem_addr=malloc(512);
	read_mem_addr=malloc(512);


//	write_params[0]=1;
//	write_params[1]=10;
//	write_params[2]=write_mem_addr;
//	SYSCALL(25,write_params);

//	write_params[0]=1;
//	write_params[1]=10;
//	write_params[2]=write_mem_addr;
//	SYSCALL(25,write_params);

	
	read_params[0]=1;
	read_params[1]=10;
	read_params[2]=read_mem_addr;
	SYSCALL(24,read_params);

//	read_params[0]=1;
//	read_params[1]=10;
//	read_params[2]=read_mem_addr;
//	SYSCALL(24,read_params);

	free(write_mem_addr);
	free(read_mem_addr);
	exit(0);
}
