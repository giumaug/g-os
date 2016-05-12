#include "lib/lib.h"
#include "udpclient.h"

//#include <stdio.h>
//#include <unistd.h>
//#include <stdlib.h>
//#include <string.h>
//#include <netdb.h>
//#include <sys/types.h> 
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

#define BUFSIZE 1024
#define IP(a,b,c,d)	(d | (c<<8) | (b<<16) | (a<<24))

const char* source_files[] = 
{
	"/home/peppe/Scrivania/g-os/syscall_handler.c",
	"/home/peppe/Scrivania/g-os/console/console.c",
	"/home/peppe/Scrivania/g-os/process/lib/stat.c",
	"/home/peppe/Scrivania/g-os/process/lib/stdio.c",
	"/home/peppe/Scrivania/g-os/process/lib/string.c",
	"/home/peppe/Scrivania/g-os/process/lib/math.c",
	"/home/peppe/Scrivania/g-os/process/lib/debug.c",
	"/home/peppe/Scrivania/g-os/process/lib/stdlib.c",
	"/home/peppe/Scrivania/g-os/process/lib/malloc.c",
	"/home/peppe/Scrivania/g-os/process/lib/fcntl.c",
	"/home/peppe/Scrivania/g-os/process/lib/unistd.c",
	"/home/peppe/Scrivania/g-os/process/cd.c",
	"/home/peppe/Scrivania/g-os/process/ls.c",
	"/home/peppe/Scrivania/g-os/process/shell.c",
	"/home/peppe/Scrivania/g-os/data_types/queue.c",
	"/home/peppe/Scrivania/g-os/data_types/stack.c",
	"/home/peppe/Scrivania/g-os/data_types/dllist.c",
	"/home/peppe/Scrivania/g-os/data_types/hashtable.c",
	"/home/peppe/Scrivania/g-os/lib/string.c",
	"/home/peppe/Scrivania/g-os/lib/math.c",
	"/home/peppe/Scrivania/g-os/lib/kmemcpy.c",
	"/home/peppe/Scrivania/g-os/lib/printk.c",
	"/home/peppe/Scrivania/g-os/elf_loader/elf_loader.c",
	"/home/peppe/Scrivania/g-os/memory_manager/fixed_size.c",
	"/home/peppe/Scrivania/g-os/memory_manager/kmalloc.c",
	"/home/peppe/Scrivania/g-os/memory_manager/test_list.c",
	"/home/peppe/Scrivania/g-os/memory_manager/mem_proc.c",
	"/home/peppe/Scrivania/g-os/memory_manager/buddy.c",
	"/home/peppe/Scrivania/g-os/memory_manager/allocator_stress_test.c",
	"/home/peppe/Scrivania/g-os/scheduler/scheduler.c",
	"/home/peppe/Scrivania/g-os/debug.c",
	"/home/peppe/Scrivania/g-os/process_0.c",
	"/home/peppe/Scrivania/g-os/ext2/ext2.c",
	"/home/peppe/Scrivania/g-os/idt.c",
	"/home/peppe/Scrivania/g-os/memory_region/mem_regs.c",
	"/home/peppe/Scrivania/g-os/virtual_memory/vm.c",
	"/home/peppe/Scrivania/g-os/synchro_types/semaphore.c",
	"/home/peppe/Scrivania/g-os/drivers/kbc/8042.c",
	"/home/peppe/Scrivania/g-os/drivers/ata/ata.c",
	"/home/peppe/Scrivania/g-os/drivers/pic/8259A.c",
	"/home/peppe/Scrivania/g-os/drivers/pit/8253.c",
	"/home/peppe/Scrivania/g-os/kmain.c",
	"/home/peppe/Scrivania/g-os/include/data_types/hashtable.h",
	"/home/peppe/Scrivania/g-os/include/data_types/queue.h",
	"/home/peppe/Scrivania/g-os/include/data_types/primitive_types.h",
	"/home/peppe/Scrivania/g-os/include/data_types/dllist.h",
	"/home/peppe/Scrivania/g-os/include/lib/lib.h",
	"/home/peppe/Scrivania/g-os/include/memory_region/mem_regs.h",
	"/home/peppe/Scrivania/g-os/include/idt.h",
	"/home/peppe/Scrivania/g-os/include/memory_manager/buddy.h",
	"/home/peppe/Scrivania/g-os/include/memory_manager/mem_proc.h",
	"/home/peppe/Scrivania/g-os/include/memory_manager/kmalloc.h",
	"/home/peppe/Scrivania/g-os/include/memory_manager/general.h",
	"/home/peppe/Scrivania/g-os/include/memory_manager/fixed_size.h",
	"/home/peppe/Scrivania/g-os/include/syscall_handler.h",
	"/home/peppe/Scrivania/g-os/include/virtual_memory/vm.h",
	"/home/peppe/Scrivania/g-os/include/lib_/string.h",
	"/home/peppe/Scrivania/g-os/include/lib_/sys/stat.h",
	"/home/peppe/Scrivania/g-os/include/lib_/stdlib.h",
	"/home/peppe/Scrivania/g-os/include/lib_/math.h",
	"/home/peppe/Scrivania/g-os/include/lib_/fcntl.h",
	"/home/peppe/Scrivania/g-os/include/lib_/stdio.h",
	"/home/peppe/Scrivania/g-os/include/lib_/unistd.h",
	"/home/peppe/Scrivania/g-os/include/general.h",
	"/home/peppe/Scrivania/g-os/include/synchro_types/spin_lock.h",
	"/home/peppe/Scrivania/g-os/include/synchro_types/semaphore.h",
	"/home/peppe/Scrivania/g-os/include/scheduler/process.h",
	"/home/peppe/Scrivania/g-os/include/scheduler/scheduler.h",
	"/home/peppe/Scrivania/g-os/include/ext2/ext2_utils_2.h",
	"/home/peppe/Scrivania/g-os/include/ext2/ext2_utils_1.h",
	"/home/peppe/Scrivania/g-os/include/ext2/ext2.h",
	"/home/peppe/Scrivania/g-os/include/asm.h",
	"/home/peppe/Scrivania/g-os/include/process_0.h",
	"/home/peppe/Scrivania/g-os/include/drivers/pic/8259A.h",
	"/home/peppe/Scrivania/g-os/include/drivers/pit/8253.h",
	"/home/peppe/Scrivania/g-os/include/drivers/kbc/8042.h",
	"/home/peppe/Scrivania/g-os/include/drivers/ata/ata.h",
	"/home/peppe/Scrivania/g-os/include/system.h",
	"/home/peppe/Scrivania/g-os/include/debug.h",
	"/home/peppe/Scrivania/g-os/include/elf_loader/elf_loader.h",
	"/home/peppe/Scrivania/g-os/include/console/console.h",
	"/home/peppe/Scrivania/g-os/process/lib/lib.h",
	"/home/peppe/Scrivania/g-os/process/lib/malloc.h",
	"/home/peppe/Scrivania/g-os/process/shell.h",
	"/home/peppe/Scrivania/g-os/process/ls.h"
};

int main(int argc, char **argv) 
{
	typedef int u32;
	char* reader="./udp_reader";
	u32 source_id,n_request,ret,age,i,pid;
	
	argv=malloc(sizeof(char*)*2);
	argv[0]=reader;
	age=0;

	while (1)
	{
		n_request=(rand() % 10 + 1);
		printf("\nage=%d \n",age);
		printf("\nrequest number=%d \n",n_request);

		for (i=0;i<n_request;i++)
		{
			pid=fork();
			if (pid==0)
			{
				source_id=(rand() % 83 + 1);
				argv[1]=source_files[source_id];
				ret=exec(argv[0],argv);
			}
		}
		age++;
		sleep(1);
		//check_free_mem();
	}   
    	return 0;
}
