#include "lib/lib.h"  
#include "webserver.h"

#define NUMFILE 20982

const char * array[] = 
{
	"/usr/src/kernels/g-os/g-os/syscall_handler.c",
	"/usr/src/kernels/g-os/g-os/console/console.c",
	"/usr/src/kernels/g-os/g-os/process/mem-stress.c",
	"/usr/src/kernels/g-os/g-os/process/forker.c",
	"/usr/src/kernels/g-os/g-os/process/lib/stat.c",
	"/usr/src/kernels/g-os/g-os/process/lib/stdio.c",
	"/usr/src/kernels/g-os/g-os/process/lib/string.c",
	"/usr/src/kernels/g-os/g-os/process/lib/math.c",
	"/usr/src/kernels/g-os/g-os/process/lib/debug.c",
	"/usr/src/kernels/g-os/g-os/process/lib/stdlib.c",
	"/usr/src/kernels/g-os/g-os/process/lib/malloc.c",
	"/usr/src/kernels/g-os/g-os/process/lib/fcntl.c",
	"/usr/src/kernels/g-os/g-os/process/lib/unistd.c",
	"/usr/src/kernels/g-os/g-os/process/cd.c",
	"/usr/src/kernels/g-os/g-os/process/fork-wrapper.c",
	"/usr/src/kernels/g-os/g-os/process/process1.c",
	"/usr/src/kernels/g-os/g-os/process/ls.c",
	"/usr/src/kernels/g-os/g-os/process/shell.c",
	"/usr/src/kernels/g-os/g-os/process/selection-sort.c",
	"/usr/src/kernels/g-os/g-os/data_types/queue.c",
	"/usr/src/kernels/g-os/g-os/data_types/stack.c",
	"/usr/src/kernels/g-os/g-os/data_types/dllist.c",
	"/usr/src/kernels/g-os/g-os/data_types/hashtable.c",
	"/usr/src/kernels/g-os/g-os/lib/string.c",
	"/usr/src/kernels/g-os/g-os/lib/math.c",
	"/usr/src/kernels/g-os/g-os/lib/kmemcpy.c",
	"/usr/src/kernels/g-os/g-os/lib/printk.c",
	"/usr/src/kernels/g-os/g-os/elf_loader/elf_loader.c",
	"/usr/src/kernels/g-os/g-os/memory_manager/fixed_size.c",
	"/usr/src/kernels/g-os/g-os/memory_manager/kmalloc.c",
	"/usr/src/kernels/g-os/g-os/memory_manager/test_list.c",
	"/usr/src/kernels/g-os/g-os/memory_manager/mem_proc.c",
	"/usr/src/kernels/g-os/g-os/memory_manager/buddy.c",
	"/usr/src/kernels/g-os/g-os/memory_manager/allocator_stress_test.c",
	"/usr/src/kernels/g-os/g-os/scheduler/scheduler.c",
	"/usr/src/kernels/g-os/g-os/debug.c",
	"/usr/src/kernels/g-os/g-os/process_0.c",
	"/usr/src/kernels/g-os/g-os/ext2/ext2.c",
	"/usr/src/kernels/g-os/g-os/idt.c",
	"/usr/src/kernels/g-os/g-os/memory_region/mem_regs.c",
	"/usr/src/kernels/g-os/g-os/virtual_memory/vm.c",
	"/usr/src/kernels/g-os/g-os/synchro_types/semaphore.c",
	"/usr/src/kernels/g-os/g-os/drivers/kbc/8042.c",
	"/usr/src/kernels/g-os/g-os/drivers/ata/ata.c",
	"/usr/src/kernels/g-os/g-os/drivers/pic/8259A.c",
	"/usr/src/kernels/g-os/g-os/drivers/pit/8253.c",
	"/usr/src/kernels/g-os/g-os/kmain.c"

int main (int argc, char* argv[])
{

}

1 408 153
