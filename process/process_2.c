#include "lib/stdio.h" 
#include "lib/stdlib.h" 

#define SEED 212430

process_2()
{
	unsigned int y,z;
	unsigned char* write_mem_addr;
	unsigned char* read_mem_addr;
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
//		_write_28_ata(system->device_desc,1,10,mem_addr);
//
//		free(mem_addr);
//	}
	write_mem_addr=malloc(512);
	read_mem_addr=malloc(512);
	_write_28_ata(system->device_desc,1,10,mem_addr);
	_read_28_ata(system->device_desc,1,10,mem_addr);
	free(write_mem_addr);
	free(read_mem_addr);
	exit(0);
}
