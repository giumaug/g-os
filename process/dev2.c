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
	int hash = 0;

	io_buffer_a = malloc(1024);
	io_buffer_b = malloc(1024);

	for (j = 0; j < 512; j++)
	{
		io_buffer_a[j] = 'a';
		io_buffer_b[j] = 'b';
	}
	
//	for ( j = 0 ; j < 10; j++)
//	{
		printf("starting process!!!!!!! \n");
		//printf("iteration %d \n", j);
		f = open(argv[1], O_CREAT | O_RDWR);                                      
		//for (i = 0; i < 30721; i++)
		for (i = 0; i < 3080; i++)
		{
			if (i % 10000 == 0)
			{
				//printf("count is %d \n",i);
				//check_free_mem();
			}
			tot_read += write(f,io_buffer_a, 512);
			tot_read += write(f,io_buffer_b, 512);
			for (j = 0; j < 512; j++)
			{
				hash += io_buffer_a[j];
				hash += io_buffer_b[j];
			}
			
		}
		close(f);
//		if (j < 9)
//		{
//			remove(argv[1]);
//		}
//	}
	free(io_buffer_a);
	free(io_buffer_b);
	printf("end process... %d \n",tot_read);
	printf("hash is %d \n", hash);
	//flush_inode_cache();
	exit(0);
}
