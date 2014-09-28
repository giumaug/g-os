#include "lib/lib.h"

main()
{
	int i,f;
        char* io_buffer;
	
	io_buffer=malloc(18737);
	f=open("/tmp/readme.txt", O_RDWR | O_APPEND);
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



