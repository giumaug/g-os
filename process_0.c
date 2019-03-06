#include "asm.h"
#include "lib/lib.h"
#include "scheduler/scheduler.h"
#include "virtual_memory/vm.h"
#include "process_0.h"

//_exec SWITCH KERNEL THREAD TO USERSPACE PROCESS
// THREAD_FORK CLONE KERNEL THREAD
// THREAD_EXEC CREATE NEW KERNEL THREAD TO BE IMPLEMENTED!!!!!!!!!
void process_0() 
{
	unsigned int pid;
	static char* argv[2];
	static char argv1[]="/shell";	
	
	argv[0]=argv1;
	argv[1]=NULL;	

    THREAD_FORK(pid);
	if (pid==0)
	{
		printk("before...\n");
/*
		t_io_request* io_request = kmalloc(sizeof(t_io_request));
		io_request->lba = 2052;
		io_request->sector_count = 1;
		io_request->command = 32;
		init_ata_test();
		
		out(0xE0 | (io_request->lba >> 24),0x1F6);
		out((unsigned char)io_request->sector_count,0x1F2);
		out((unsigned char)io_request->lba,0x1F3);
		out((unsigned char)(io_request->lba >> 8),0x1F4);
		out((unsigned char)(io_request->lba >> 16),0x1F5);
		out(io_request->command,0x1F7);

		//___read_write_28_ata(io_request);
		while(1);
*/
		_exec("/shell",argv);	
		printk("after... \n");
	}
	else 
	{
		THREAD_EXIT(0);
	}
}
