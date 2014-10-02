#include "lib/lib.h"  

int main(int argc, char *argv[])
{
	printf("\n");
	printf("argc=");
	print_num(argc);
	printf("\n");
	printf("argv[0]=");
	printf(argv[0]);
	printf("\n");

	read(f,io_buffer,18736);
	close(f);
	
	if (argc>1)
	{
		f=open(argv[1], O_RDWR | O_APPEND);
	}
	else 
	{
		//nedd to propagate from parent to child look current_dir_inode_number inside lookup_inode
		f=open("./", O_RDWR | O_APPEND);
	}




	exit(0);
}
