#include "lib/lib.h"  
#include "reader.h"

int main (int argc, char* argv[])
{
	int i,f;
        char* io_buffer;
	t_stat stat_data;
//	char* tmp="/usr/src/kernels/g-os/include/memory_manager/mem_proc.h";
	
	printf("argv[1]=%s \n",argv[1]);
	
	stat(argv[1],&stat_data);
//	stat(tmp,&stat_data);
	io_buffer=malloc(stat_data.st_size+1);
	f=open(argv[1], O_RDWR | O_APPEND);
//	f=open(tmp, O_RDWR | O_APPEND);
	read(f,io_buffer,stat_data.st_size+1);
	close(f);
	io_buffer[stat_data.st_size+1]='\0';
	printf(io_buffer);
	free(io_buffer);
	exit(0);
}
