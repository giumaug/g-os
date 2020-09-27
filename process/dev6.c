#include "lib/lib.h"

int main(int argc, char* argv[])
{
	int i;
	int f;
	int tot_read = 0;
	char buffer[4096];

	printf("starting process \n");
	//buffer = malloc(4096);
	for (i = 0; i < 4096; i++)
	{
		buffer[i] = 'a';
	}

	f = open(argv[1], O_CREAT | O_RDWR);
	for (i = 0; i < 61440; i++)
	//for (i = 0; i < 7680; i++)
	{	
		//printf("index is %d \n", i);
		tot_read += write(f,buffer,512);
		
	}
	close(f);
	printf("end process %d \n",tot_read);
	//remove(argv[1]);
	exit(0);
}
