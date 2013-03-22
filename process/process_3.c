#include "lib/stdio.h"
#include "lib/unistd.h"
#include "lib/stdlib.h"
#include "process/process_1.h"

extern  t_data data[3];

void  process_3() 
{
	int i,j;

	printf("start process_3 \n");
	for (i=0;i<data[1].iteration;i++)
	{
		sleep(data[1].io_time);
		for (j=1;j<data[1].cpu_time;j++);
	}
	printf("end process_3 \n");
	exit(0);
}



