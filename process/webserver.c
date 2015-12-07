#include "lib/lib.h"  
#include "webserver.h"

const char* source_files[] = 
{
	"/usr/src/kernels/g-os/syscall_handler.c",
	"/usr/src/kernels/g-os/console/console.c",
	"/usr/src/kernels/g-os/process/lib/stat.c",
	"/usr/src/kernels/g-os/process/lib/stdio.c",
	"/usr/src/kernels/g-os/process/lib/string.c",
	"/usr/src/kernels/g-os/process/lib/math.c",
	"/usr/src/kernels/g-os/process/lib/debug.c",
	"/usr/src/kernels/g-os/process/lib/stdlib.c",
	"/usr/src/kernels/g-os/process/lib/malloc.c",
	"/usr/src/kernels/g-os/process/lib/fcntl.c",
	"/usr/src/kernels/g-os/process/lib/unistd.c",
	"/usr/src/kernels/g-os/process/cd.c",
	"/usr/src/kernels/g-os/process/ls.c",
	"/usr/src/kernels/g-os/process/shell.c",
	"/usr/src/kernels/g-os/data_types/queue.c",
	"/usr/src/kernels/g-os/data_types/stack.c",
	"/usr/src/kernels/g-os/data_types/dllist.c",
	"/usr/src/kernels/g-os/data_types/hashtable.c",
	"/usr/src/kernels/g-os/lib/string.c",
	"/usr/src/kernels/g-os/lib/math.c",
	"/usr/src/kernels/g-os/lib/kmemcpy.c",
	"/usr/src/kernels/g-os/lib/printk.c",
	"/usr/src/kernels/g-os/elf_loader/elf_loader.c",
	"/usr/src/kernels/g-os/memory_manager/fixed_size.c",
	"/usr/src/kernels/g-os/memory_manager/kmalloc.c",
	"/usr/src/kernels/g-os/memory_manager/test_list.c",
	"/usr/src/kernels/g-os/memory_manager/mem_proc.c",
	"/usr/src/kernels/g-os/memory_manager/buddy.c",
	"/usr/src/kernels/g-os/memory_manager/allocator_stress_test.c",
	"/usr/src/kernels/g-os/scheduler/scheduler.c",
	"/usr/src/kernels/g-os/debug.c",
	"/usr/src/kernels/g-os/process_0.c",
	"/usr/src/kernels/g-os/ext2/ext2.c",
	"/usr/src/kernels/g-os/idt.c",
	"/usr/src/kernels/g-os/memory_region/mem_regs.c",
	"/usr/src/kernels/g-os/virtual_memory/vm.c",
	"/usr/src/kernels/g-os/synchro_types/semaphore.c",
	"/usr/src/kernels/g-os/drivers/kbc/8042.c",
	"/usr/src/kernels/g-os/drivers/ata/ata.c",
	"/usr/src/kernels/g-os/drivers/pic/8259A.c",
	"/usr/src/kernels/g-os/drivers/pit/8253.c",
	"/usr/src/kernels/g-os/kmain.c",
	"/usr/src/kernels/g-os/include/data_types/hashtable.h",
	"/usr/src/kernels/g-os/include/data_types/queue.h",
	"/usr/src/kernels/g-os/include/data_types/primitive_types.h",
	"/usr/src/kernels/g-os/include/data_types/dllist.h",
	"/usr/src/kernels/g-os/include/lib/lib.h",
	"/usr/src/kernels/g-os/include/memory_region/mem_regs.h",
	"/usr/src/kernels/g-os/include/idt.h",
	"/usr/src/kernels/g-os/include/memory_manager/buddy.h",
	"/usr/src/kernels/g-os/include/memory_manager/mem_proc.h",
	"/usr/src/kernels/g-os/include/memory_manager/kmalloc.h",
	"/usr/src/kernels/g-os/include/memory_manager/general.h",
	"/usr/src/kernels/g-os/include/memory_manager/fixed_size.h",
	"/usr/src/kernels/g-os/include/syscall_handler.h",
	"/usr/src/kernels/g-os/include/virtual_memory/vm.h",
	"/usr/src/kernels/g-os/include/lib_/string.h",
	"/usr/src/kernels/g-os/include/lib_/sys/stat.h",
	"/usr/src/kernels/g-os/include/lib_/stdlib.h",
	"/usr/src/kernels/g-os/include/lib_/math.h",
	"/usr/src/kernels/g-os/include/lib_/fcntl.h",
	"/usr/src/kernels/g-os/include/lib_/stdio.h",
	"/usr/src/kernels/g-os/include/lib_/unistd.h",
	"/usr/src/kernels/g-os/include/general.h",
	"/usr/src/kernels/g-os/include/synchro_types/spin_lock.h",
	"/usr/src/kernels/g-os/include/synchro_types/semaphore.h",
	"/usr/src/kernels/g-os/include/scheduler/process.h",
	"/usr/src/kernels/g-os/include/scheduler/scheduler.h",
	"/usr/src/kernels/g-os/include/ext2/ext2_utils_2.h",
	"/usr/src/kernels/g-os/include/ext2/ext2_utils_1.h",
	"/usr/src/kernels/g-os/include/ext2/ext2.h",
	"/usr/src/kernels/g-os/include/asm.h",
	"/usr/src/kernels/g-os/include/process_0.h",
	"/usr/src/kernels/g-os/include/drivers/pic/8259A.h",
	"/usr/src/kernels/g-os/include/drivers/pit/8253.h",
	"/usr/src/kernels/g-os/include/drivers/kbc/8042.h",
	"/usr/src/kernels/g-os/include/drivers/ata/ata.h",
	"/usr/src/kernels/g-os/include/system.h",
	"/usr/src/kernels/g-os/include/debug.h",
	"/usr/src/kernels/g-os/include/elf_loader/elf_loader.h",
	"/usr/src/kernels/g-os/include/console/console.h",
	"/usr/src/kernels/g-os/process/lib/lib.h",
	"/usr/src/kernels/g-os/process/lib/malloc.h",
	"/usr/src/kernels/g-os/process/shell.h",
	"/usr/src/kernels/g-os/process/ls.h"
};

int main (int argc, char* argv[])
{
	int i;
	int ret;
	int source_id;
	int n_request;
	int age;
	int pid;

	char* reader="/reader";
	argv=malloc(sizeof(char*)*2);
	argv[0]=reader;
	age=0;

	while(1)
	{
		n_request=(rand() % 15 + 1);

		printf("\nage=%d \n",age);
		printf("\nrequest number=%d \n",n_request);
		sleep(5000);

		//source_id=1;
		//n_request=1;

		for (i=0;i<n_request;i++)
		{
			source_id=(rand() % 83 + 1);
			pid=fork();
			if (pid==0)
			{
				argv[1]=source_files[source_id];
				ret=exec(argv[0],argv);
			}
		}
		age++;
		sleep(20000);
		check_free_mem();
	}
	exit(0);
}

