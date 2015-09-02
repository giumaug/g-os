#include "lib/lib.h"  
#include "reader.h"

int main (int argc, char* argv[])
{
	int i,f;
        char* io_buffer;
	
	io_buffer=malloc(30000);
	f=open(argv[1], O_RDWR | O_APPEND);
	read(f,io_buffer,18736);
	close(f);
	printf("\n");
	printf("hello !!!!!!");
	io_buffer[18737]='\0';
	printf("\n");
	printf(io_buffer);
	printf("\n");
	free(io_buffer);
	exit(0);
}
