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
//#include <sys/stat.h>
//#include <fcntl.h>

#define PACKET_SIZE 100
#define BUFSIZE 1024
#define IP(a,b,c,d)	(d | (c<<8) | (b<<16) | (a<<24))

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

typedef unsigned int u32;
typedef unsigned short u16;

int main(int argc, char **argv) 
{
	int sockfd,sockfd_1,n_to,i,f,len,send_len;
    	struct sockaddr_in send_addr;
    	char send_buf[BUFSIZE];
        char* io_buffer;
	t_stat stat_data;
	//struct stat stat_data;
	u16 port,fixed_port;
	u32 len_to_send,sent;
	int data_len,offset;
	char* end="!z!";
	char* filename=NULL;
	u32 source_id;

	printf("argc=%d \n",argc);	
	printf("starting udp_writer... \n");
	unsigned char a1=argv[1][0];
	unsigned char a2=argv[1][1];
	source_id=((a1<<8)+a2)-255;
	port=21000;
	
	printf("a1 id=%d \n",a1);
	printf("a2=%d \n",a2);
	printf("source id=%d \n",source_id);
	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[2]=6;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[3]=1;
	((unsigned char*) &(send_addr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(send_addr.sin_port))[1]=((unsigned char*) &(port))[0];

	//source_id=(rand() % 83 + 1);
	filename=source_files[source_id];
	stat(filename,&stat_data);
	data_len=stat_data.st_size+1;
	io_buffer=malloc(data_len);
	printf("data len is %d \n",data_len);
	f=open(filename, O_RDWR | O_APPEND);
	read(f,io_buffer,data_len);
	close(f);
	io_buffer[data_len]='\0';

	send_len = sizeof(send_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	sent=1;
	offset=-PACKET_SIZE;
	while (sent!=0)
	{
		offset+=PACKET_SIZE;
		int ff=data_len-(offset+PACKET_SIZE);
		if (data_len-(offset+PACKET_SIZE)>=0)
		{
			len=PACKET_SIZE;
		}
		else
		{
			len=data_len-offset;
			sent=0;
		}
		printf("sending data: %d \n",len);
    		n_to = sendto(sockfd,(io_buffer+offset),len,0,&send_addr, send_len);
		printf("sent data \n");
	}
	close_socket(sockfd);
    	exit(0);
}
