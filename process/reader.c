#include "lib/lib.h"  
#include "reader.h"

int main (int argc, char* argv[])
{
	int i,f;
        char* io_buffer;
	t_stat stat;
	
	stat(argv[0],&stat);
	io_buffer=malloc(stat.st_size+1);
	f=open(argv[1], O_RDWR | O_APPEND);
	read(f,io_buffer,stat.st_size);
	close(f);
	
	io_buffer[stat.st_size+1]='\0';
	printf(io_buffer);
	free(io_buffer);
	exit(0);
}
