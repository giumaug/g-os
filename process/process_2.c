#include "lib/stdio.h" 
#include "lib/stdlib.h"
#include "system.h"

extern t_system system;

process_2()
{
	unsigned int pool_index;
	unsigned int buddy_index;
	unsigned int z,y;
	void* address[4000];
	unsigned int write_to;
	unsigned int buddy_mem;
	unsigned int pool_mem;

	pool_index=(rand() % 16 + 1);
	buddy_index=(rand() % 16 + 1);

	unsigned int pid=((struct t_process_context*)system.process_info.current_process->val)->pid;
	
	for (y=0;y<1000;y++)
	{
		pool_index=5;
		
		address[y]=malloc(pool_index);
		for (z=0;z<pool_index;z++)
		{
			*((char*)address[y]+z)=0x55;
		}
		pool_mem=kfree_mem();
		printf("\npid=");
		d_printf(pid);
		printf(" free meme pool mem=");
		d_printf(pool_mem);
		printf("\n");
	}
	for (y=0;y<1000;y++)
	{
		free(address[y]);
	}

	for (y=0;y<8;y++)
	{
		buddy_index=(rand() % 4194304);

		
		address[y]=bigMalloc(buddy_index);
		for (z=0;z<buddy_index;z++)
		{
			*((char*)address[y]+z)=0x55;
		}
		buddy_mem=buddy_free_mem(&system.buddy_desc);
		//buddy_mem=0;
		printf("\npid=");
		d_printf(pid);
		printf(" free buddy mem=");
		d_printf(buddy_mem);
		printf("\n");
	}
	for (y=0;y<8;y++)
	{
		bigFree(address[y]);
	}
	exit(0);
}
