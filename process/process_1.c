#include "lib/unistd.h"
#include "lib/stdio.h" 
#include "lib/stdlib.h"
#include "system.h"

extern t_system system;

process_1()
{
	unsigned int n_request;
	unsigned int i;
	unsigned int age=0;
	unsigned int pid;
	
	while(1)
	{
		printf("\nage=");
		d_printf(age);
		printf("\n");

		for (i=0;i<10;i++)
		{	
			pid=fork();
			if (pid==0)
			{	
				exec(0x700000,0x100000);
			}
		}
		age++;
		sleep(30000);
		check_free_mem();
	}
	exit(0);
}


/*
process_1()
{
	unsigned int pool_index;
	unsigned int buddy_index;
	unsigned int z,y;
	void* address[4000];
	//unsigned int mem[20][2];
	unsigned int write_to;
	unsigned int buddy_mem;
	unsigned int pool_mem;

	pool_index=(rand() % 16 + 1);
	buddy_index=(rand() % 16 + 1);

	unsigned int pid=((struct t_process_context*)system.process_info.current_process->val)->pid;
	
	buddy_mem=buddy_free_mem(&system.buddy_desc);
	printf("\n free buddy mem=");
	d_printf(buddy_mem);
	printf("\n");
	while(1)
	{
		for (y=0;y<15;y++)
		{
			buddy_index=(rand() % 4194304);
		
			address[y]=bigMalloc(buddy_index);
			for (z=0;z<buddy_index;z++)
			{
				*((char*)address[y]+z)=0x55;
			}
			buddy_mem=buddy_free_mem(&system.buddy_desc);
//			mem[y][0]=buddy_index;
//			mem[y][1]=buddy_mem;
			printf("\npid=");
			d_printf(pid);
			printf(" free buddy mem=");
			d_printf(buddy_mem);
			printf(" requested size=");
			d_printf(buddy_index);
			printf("\n");
		}
		printf("..............................\n");
		for (y=0;y<15;y++)
		{
			buddy_mem=buddy_free_mem(&system.buddy_desc);
			bigFree(address[14-y]);
//			printf("\npid=");
//			d_printf(pid);
//			printf(" free buddy mem=");
//			d_printf(buddy_mem);
//			printf(" requested size=");
//			d_printf(mem[14-y][0]);
//			printf("\n");
//			if (buddy_mem!=mem[14-y][1])
//			{
//				panic();
//			}
		}
		sleep(5000);
		check_free_mem();
	}
	exit(0);
}
*/
