#include "lib/lib.h"

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

const char* source_files[] = 
{
	"/usr/src/kernels/g-os/network/xxx.c",
	"/usr/src/kernels/g-os/network/udp.c",
	"/usr/src/kernels/g-os/idt.c",
	"/usr/src/kernels/g-os/asm.s",
	"/usr/src/kernels/g-os/memory_manager/allocator_stress_test.c",
	"/usr/src/kernels/g-os/memory_manager/kmalloc.c",
	"/usr/src/kernels/g-os/memory_manager/mem_proc.c",
	"/usr/src/kernels/g-os/memory_manager/fixed_size.c",
	"/usr/src/kernels/g-os/memory_manager/buddy.c",
	"/usr/src/kernels/g-os/memory_manager/test_list.c",
	"/usr/src/kernels/g-os/memory_manager/Makefile",
	"/usr/src/kernels/g-os/process_0.c",
	"/usr/src/kernels/g-os/memory_region/mem_regs.c",
	"/usr/src/kernels/g-os/memory_region/Makefile",
	"/usr/src/kernels/g-os/console/console.c",
	"/usr/src/kernels/g-os/console/Makefile",
	"/usr/src/kernels/g-os/include/system.h",
	"/usr/src/kernels/g-os/include/memory_manager/buddy.h",
	"/usr/src/kernels/g-os/include/memory_manager/mem_proc.h",
	"/usr/src/kernels/g-os/include/memory_manager/kmalloc.h",
	"/usr/src/kernels/g-os/include/memory_manager/general.h",
	"/usr/src/kernels/g-os/include/memory_manager/fixed_size.h",
	"/usr/src/kernels/g-os/include/memory_region/mem_regs.h",
	"/usr/src/kernels/g-os/include/console/console.h",
	"/usr/src/kernels/g-os/include/timer.h",
	"/usr/src/kernels/g-os/include/process_0.h",
	"/usr/src/kernels/g-os/include/pci/pci.h",
	"/usr/src/kernels/g-os/include/debug.h",
	"/usr/src/kernels/g-os/include/ext2/ext2.h",
	"/usr/src/kernels/g-os/include/ext2/ext2_utils_1.h",
	"/usr/src/kernels/g-os/include/ext2/ext2_utils_2.h",
	"/usr/src/kernels/g-os/include/asm.h",
	"/usr/src/kernels/g-os/include/synchro_types/spin_lock.h",
	"/usr/src/kernels/g-os/include/synchro_types/semaphore.h",
	"/usr/src/kernels/g-os/include/scheduler/scheduler.h",
	"/usr/src/kernels/g-os/include/scheduler/process.h",
	"/usr/src/kernels/g-os/include/elf_loader/elf_loader.h",
	"/usr/src/kernels/g-os/include/syscall_handler.h",
	"/usr/src/kernels/g-os/include/common.h",
	"/usr/src/kernels/g-os/include/drivers/i8254x/i8254x.h",
	"/usr/src/kernels/g-os/include/drivers/rtl8139/rtl8139.h",
	"/usr/src/kernels/g-os/include/drivers/pic/8259A.h",
	"/usr/src/kernels/g-os/include/drivers/ata/ata.h",
	"/usr/src/kernels/g-os/include/drivers/pit/8253.h",
	"/usr/src/kernels/g-os/include/drivers/kbc/8042.h",
	"/usr/src/kernels/g-os/include/idt.h",
	"/usr/src/kernels/g-os/include/data_types/primitive_types.h",
	"/usr/src/kernels/g-os/include/data_types/hashtable.h",
	"/usr/src/kernels/g-os/include/data_types/queue.h",
	"/usr/src/kernels/g-os/include/data_types/bit_vector.h",
	"/usr/src/kernels/g-os/include/data_types/dllist.h",
	"/usr/src/kernels/g-os/include/network/socket.h",
	"/usr/src/kernels/g-os/include/network/network.h",
	"/usr/src/kernels/g-os/include/network/ip4.h",
	"/usr/src/kernels/g-os/include/network/udp.h",
	"/usr/src/kernels/g-os/include/network/common.h",
	"/usr/src/kernels/g-os/include/network/tcp_conn_map.h",
	"/usr/src/kernels/g-os/include/network/tcp.h",
	"/usr/src/kernels/g-os/include/network/arp.h",
	"/usr/src/kernels/g-os/include/network/socket_buffer.h",
	"/usr/src/kernels/g-os/include/network/mac.h",
	"/usr/src/kernels/g-os/include/network/tcp_socket.h",
	"/usr/src/kernels/g-os/include/lib/lib.h",
	"/usr/src/kernels/g-os/include/general.h",
	"/usr/src/kernels/g-os/include/virtual_memory/vm.h",
	"/usr/src/kernels/g-os/start_bridge.sh",
	"/usr/src/kernels/g-os/pci/pci.c",
	"/usr/src/kernels/g-os/pci/Makefile",
	"/usr/src/kernels/g-os/make.rules",
	"/usr/src/kernels/g-os/ext2/ext2.c",
	"/usr/src/kernels/g-os/ext2/Makefile",
	"/usr/src/kernels/g-os/timer.c",
	"/usr/src/kernels/g-os/synchro_types/semaphore.c",
	"/usr/src/kernels/g-os/synchro_types/Makefile",
	"/usr/src/kernels/g-os/scheduler/scheduler.c",
	"/usr/src/kernels/g-os/scheduler/Makefile",
	"/usr/src/kernels/g-os/elf_loader/elf_loader.c",
	"/usr/src/kernels/g-os/elf_loader/Makefile",
	"/usr/src/kernels/g-os/drivers/i8254x/i8254x.c",
	"/usr/src/kernels/g-os/drivers/i8254x/Makefile",
	"/usr/src/kernels/g-os/drivers/rtl8139/rtl8139.c",
	"/usr/src/kernels/g-os/drivers/pic/8259A.c",
	"/usr/src/kernels/g-os/drivers/pic/Makefile",
	"/usr/src/kernels/g-os/drivers/ata/ata.c",
	"/usr/src/kernels/g-os/drivers/ata/Makefile",
	"/usr/src/kernels/g-os/drivers/pit/8253.c",
	"/usr/src/kernels/g-os/drivers/pit/Makefile",
	"/usr/src/kernels/g-os/drivers/kbc/8042.c",
	"/usr/src/kernels/g-os/drivers/kbc/Makefile",
	"/usr/src/kernels/g-os/drivers/Makefile",
	"/usr/src/kernels/g-os/kmain.c",
	"/usr/src/kernels/g-os/data_types/bit_vector.c",
	"/usr/src/kernels/g-os/data_types/stack.c",
	"/usr/src/kernels/g-os/data_types/hashtable.c",
	"/usr/src/kernels/g-os/data_types/dllist.c",
	"/usr/src/kernels/g-os/data_types/queue.c",
	"/usr/src/kernels/g-os/data_types/Makefile",
	"/usr/src/kernels/g-os/start_bridge_work.sh",
	"/usr/src/kernels/g-os/amazon_config.sh",
	"/usr/src/kernels/g-os/doc/run-g-os",
	"/usr/src/kernels/g-os/doc/disk_performance.txt",
	"/usr/src/kernels/g-os/doc/bridge.txt",
	"/usr/src/kernels/g-os/doc/kernel_memory_layout.txt",
	"/usr/src/kernels/g-os/doc/qemu_istance.txt",
	"/usr/src/kernels/g-os/doc/doc_link.txt",
	"/usr/src/kernels/g-os/doc/simics.txt",
	"/usr/src/kernels/g-os/doc/qemu_image.txt",
	"/usr/src/kernels/g-os/doc/readme.txt",
	"/usr/src/kernels/g-os/doc/qemu_configure.txt",
	"/usr/src/kernels/g-os/doc/qemu_link.txt",
	"/usr/src/kernels/g-os/network/tcp_conn_map.c",
	"/usr/src/kernels/g-os/network/tcp.c",
	"/usr/src/kernels/g-os/network/tcp_socket.c",
	"/usr/src/kernels/g-os/network/ip4.c",
	"/usr/src/kernels/g-os/network/network.c",
	"/usr/src/kernels/g-os/network/udp.c",
	"/usr/src/kernels/g-os/network/socket_buffer.c",
	"/usr/src/kernels/g-os/network/socket.c",
	"/usr/src/kernels/g-os/network/mac.c",
	"/usr/src/kernels/g-os/network/arp.c",
	"/usr/src/kernels/g-os/network/Makefile",
	"/usr/src/kernels/g-os/debug.c",
	"/usr/src/kernels/g-os/kernel_init.s",
	"/usr/src/kernels/g-os/lib/printk.c",
	"/usr/src/kernels/g-os/lib/math.c",
	"/usr/src/kernels/g-os/lib/string.c",
	"/usr/src/kernels/g-os/lib/kmemcpy.c",
	"/usr/src/kernels/g-os/lib/Makefile",
	"/usr/src/kernels/g-os/syscall_handler.c",
	"/usr/src/kernels/g-os/linker.ld",
	"/usr/src/kernels/g-os/loader.s",
	"/usr/src/kernels/g-os/virtual_memory/vm.c",
	"/usr/src/kernels/g-os/virtual_memory/Makefile",
	"/usr/src/kernels/g-os/process/shell.h",
	"/usr/src/kernels/g-os/process/network_client/c1.c",
	"/usr/src/kernels/g-os/process/network_client/c2.c",
	"/usr/src/kernels/g-os/process/network_client/c.c",
	"/usr/src/kernels/g-os/process/network_client/cot.c",
	"/usr/src/kernels/g-os/process/network_client/readme.txt",
	"/usr/src/kernels/g-os/process/process1.c",
	"/usr/src/kernels/g-os/process/udpclient.h",
	"/usr/src/kernels/g-os/process/dd.h",
	"/usr/src/kernels/g-os/process/udpclient.c",
	"/usr/src/kernels/g-os/process/hello.c",
	"/usr/src/kernels/g-os/process/cd.c",
	"/usr/src/kernels/g-os/process/net.h",
	"/usr/src/kernels/g-os/process/tt2.h",
	"/usr/src/kernels/g-os/process/selection-sort.h",
	"/usr/src/kernels/g-os/process/cd.h",
	"/usr/src/kernels/g-os/process/s.c",
	"/usr/src/kernels/g-os/process/flood.h",
	"/usr/src/kernels/g-os/process/mem-stress.c",
	"/usr/src/kernels/g-os/process/tcpclient.c",
	"/usr/src/kernels/g-os/process/cunc_udp_client.c",
	"/usr/src/kernels/g-os/process/udpwriter.h",
	"/usr/src/kernels/g-os/process/srcflood.c",
	"/usr/src/kernels/g-os/process/cot.c",
	"/usr/src/kernels/g-os/process/tcpserver.c",
	"/usr/src/kernels/g-os/process/net.c",
	"/usr/src/kernels/g-os/process/udp_reader.c",
	"/usr/src/kernels/g-os/process/cuncudpserver.h",
	"/usr/src/kernels/g-os/process/webserver.c",
	"/usr/src/kernels/g-os/process/ls.c",
	"/usr/src/kernels/g-os/process/main.c",
	"/usr/src/kernels/g-os/process/fork-wrapper.c",
	"/usr/src/kernels/g-os/process/ke2.c",
	"/usr/src/kernels/g-os/process/forker.c",
	"/usr/src/kernels/g-os/process/main.h",
	"/usr/src/kernels/g-os/process/ls.h",
	"/usr/src/kernels/g-os/process/reader.c",
	"/usr/src/kernels/g-os/process/cot.h",
	"/usr/src/kernels/g-os/process/s.h",
	"/usr/src/kernels/g-os/process/ht.h",
	"/usr/src/kernels/g-os/process/reader.h",
	"/usr/src/kernels/g-os/process/tcpclient.h",
	"/usr/src/kernels/g-os/process/ke2.h",
	"/usr/src/kernels/g-os/process/old_process/process_2.c",
	"/usr/src/kernels/g-os/process/old_process/kmalloc_test_proc_1.c",
	"/usr/src/kernels/g-os/process/old_process/process_3.c",
	"/usr/src/kernels/g-os/process/old_process/kmalloc_test_proc_3.c",
	"/usr/src/kernels/g-os/process/old_process/kmalloc_test_proc_2.c",
	"/usr/src/kernels/g-os/process/old_process/process_4.c",
	"/usr/src/kernels/g-os/process/old_process/process_1.c",
	"/usr/src/kernels/g-os/process/io.c",
	"/usr/src/kernels/g-os/process/udpserver.h",
	"/usr/src/kernels/g-os/process/udpwriter.c",
	"/usr/src/kernels/g-os/process/ht.c",
	"/usr/src/kernels/g-os/process/cuncudpserver.c",
	"/usr/src/kernels/g-os/process/fork-wrapper.h",
	"/usr/src/kernels/g-os/process/hello.h",
	"/usr/src/kernels/g-os/process/udpserver.c",
	"/usr/src/kernels/g-os/process/dd.c",
	"/usr/src/kernels/g-os/process/lib/stat.c",
	"/usr/src/kernels/g-os/process/lib/malloc.h",
	"/usr/src/kernels/g-os/process/lib/unistd.c",
	"/usr/src/kernels/g-os/process/lib/lib.h",
	"/usr/src/kernels/g-os/process/lib/stdio.c",
	"/usr/src/kernels/g-os/process/lib/stdlib.c",
	"/usr/src/kernels/g-os/process/lib/malloc.c",
	"/usr/src/kernels/g-os/process/lib/debug.c",
	"/usr/src/kernels/g-os/process/lib/socket.c",
	"/usr/src/kernels/g-os/process/lib/fcntl.c",
	"/usr/src/kernels/g-os/process/lib/math.c",
	"/usr/src/kernels/g-os/process/lib/string.c",
	"/usr/src/kernels/g-os/process/lib/Makefile",
	"/usr/src/kernels/g-os/process/tt2.c",
	"/usr/src/kernels/g-os/process/srcflood.h",
	"/usr/src/kernels/g-os/process/shell.c",
	"/usr/src/kernels/g-os/process/webserver.h",
	"/usr/src/kernels/g-os/process/linker.ld",
	"/usr/src/kernels/g-os/process/mem-stress.h",
	"/usr/src/kernels/g-os/process/sp.c",
	"/usr/src/kernels/g-os/process/process1.h",
	"/usr/src/kernels/g-os/process/flood.c",
	"/usr/src/kernels/g-os/process/selection-sort.c",
	"/usr/src/kernels/g-os/process/forker.h",
	"/usr/src/kernels/g-os/process/Makefile",
	"/usr/src/kernels/g-os/Makefile"
};

int main(int argc, char **argv) 
{
	char tmp[20];
	t_stat stat_data;
	int file_size;
	int path_size;
	int source_id;
	int len_read;
	int to_read = 4096;
	char get[1000];
	int sockfd,port,i;
    	struct sockaddr_in send_addr;
	const char get_part[] = " HTTP/1.1 \nUser-Agent: Wget/1.19.5 (linux-gnu) \nAccept: */* \n";
	char http_response[4096];
	char* sub_http_response;
	int j,jj;
	int age = 0;
	
	port = 21846;
	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[0]=192;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[1]=168;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[2]=247;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[3]=1;
	((unsigned char*) &(send_addr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(send_addr.sin_port))[1]=((unsigned char*) &(port))[0];

	printf("starting ..... \n");
	check_free_mem();
	for (j = 0; j < 1;j++) //125
	{
		age++;
		printf("age is... %d \n",age);
		if (((age % 1) == 0))
		{
			//check_free_mem();
		}
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		connect(sockfd,(struct sockaddr *) &send_addr, sizeof(send_addr));
		//source_id = (rand() % 215 + 1);
		source_id = 0; //110; //28
		path_size = strlen(source_files[source_id]);
		get[0] = 'G';
		get[1] = 'E';
		get[2] = 'T';
		get[3] = ' ';
		for(i = 0; i <= path_size - 1;i++)
		{
			get[i + 4] = source_files[source_id][i];
		}
		for(i = 0; i <= sizeof(get_part);i++)
		{
			get[i + path_size + 4] = get_part[i];
		}
		//printf("request is %s \n",get);
		write_socket(sockfd,get,(4 + sizeof(get_part) + path_size));
		len_read = read_socket(sockfd,http_response,129);//129//123
		//printf("len read is %d \n",len_read);
		jj = 0;
		do
		{
			tmp[jj] = http_response[jj + 117];
			jj++; 
		}
		while(http_response[jj + 117] != '\n');
		tmp[jj] = '\0';
		file_size = atoi(tmp);
		//printf("jjj is %d \n",jj);
		file_size -= (len_read - 116 - jj - 3);

		tmp[0] = http_response[116 + jj + 3];
		tmp[1] = http_response[116 + jj + 4];
		tmp[2] = http_response[116 + jj + 5];
		tmp[3] = http_response[116 + jj + 6];
		tmp[4] = http_response[116 + jj + 7];
		tmp[5] = '\0';
	
		//file_size = 1837;
		int val = 0;
		printf("file size is %d \n",file_size);
		int tot = 0;
		while (file_size > 0)
		{
			len_read = read_socket(sockfd,http_response,to_read);//to_read
			tot += len_read;
			//check_free_mem();
//			for ( i = 0;i < len_read;i++)
//			{
//				val += (unsigned int)http_response[i];
//				//printf("char is %s \n",http_response[i]);
//			}
//			printf("val is %d \n",val);
			//printf("data = %s ",http_response);
			//printf("read %d \n",len_read);
			//printf(" tot %d \n",tot);
			file_size -= len_read;
			//printf(" file size %d \n",file_size);
			//printf("file size is %d \n",file_size);
		}
		//printf("sending close \n");
		if (val == 216370)
		{
			printf("ok \n");
		}
		close_socket(sockfd);
	}
	printf("end \n");
    	exit(0);
}
