#include "lib/lib.h"

int main(int argc, char* argv[])
{
	int i;
	int f;
	int j;
	int tot_read = 0;
	char buffer_a[512];
	char buffer_b[512];
	int hash = 0;

	printf("starting process!!!!!!! \n");
	for (i = 0; i < 512; i++)
	{
		buffer_a[i] = 0;
		buffer_b[i] = 0;
	}

	f = open(argv[1], O_CREAT | O_RDWR);
	for (i = 0; i < 30721; i++)
	{
		tot_read += read(f,buffer_a,512);
		tot_read += read(f,buffer_b,512);
		
		for (j = 0; j < 512; j++)
		{
			hash += buffer_a[j];
			hash += buffer_b[j];
		}
	}
	close(f);
	printf("...\n");
	printf("end process %d \n",tot_read);
	printf("hash is %d \n", hash);
	exit(0);
}
