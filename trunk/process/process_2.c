#include "lib/stdio.h" 
#include "lib/stdlib.h" 

#define SEED 212430

process_2()
{
	unsigned int y,z;
	unsigned char* mem_addr;
	unsigned int workload;
	unsigned int seed=SEED;

	workload=(_rand(&seed) % 20 + 1);
	for (y=0;y<workload;y++)
	{
		mem_addr=malloc(512);
		for (z=0;z<512;z++)
		{
			*(mem_addr+z)=0xFF;
		}
		free(mem_addr);
	}
	exit(0);
}
