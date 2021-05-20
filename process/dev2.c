#include "lib/lib.h"

int main(int argc, char* argv[])
{
	int tot_read = 0;
	int f;
	int i;
	int b_read;
	char* io_buffer_a = NULL;
	char* io_buffer_b = NULL;
	char data;
	int j;

//  io_buffer_a = malloc(11);
//	io_buffer_b = malloc(11);

	io_buffer_a = malloc(1024);
	io_buffer_b = malloc(1024);

//	io_buffer_a[9] = '\n';
//	io_buffer_b[9] = '\n';

	for (j = 0; j < 512; j++)
	{
		io_buffer_a[j] = 'a';
		io_buffer_b[j] = 'b';
	}
	
	f = open(argv[1], O_CREAT | O_RDWR);
                                      
	//for (i = 0; i < 1572864; i++) //30841
	//for (i = 0; i < 786432; i++)
	//for (i = 0; i < 393216; i++)
	//for (i = 0; i < 196608; i++)
	for (i = 0; i < 30841; i++)
	{
//		data = (i % 26) + 97;
//		for (j = 0; j < 512; j++)
//		{
//			io_buffer_a[j] = data;
//			io_buffer_b[j] = data;
//		}	
		if (i % 10000 == 0)
		{
			printf("count is %d \n",i);
			check_free_mem();
		}
		tot_read += write(f,io_buffer_a, 512);
		tot_read += write(f,io_buffer_b, 512);
	}

	close(f);
	free(io_buffer_a);
	free(io_buffer_b);

	//remove(argv[1]);
	printf("end process... %d \n",tot_read);
	flush_inode_cache();
	exit(0);
}
