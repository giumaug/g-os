#include "lib/stdio.h"
#include "lib/unistd.h"
#include "lib/stdlib.h"
#include "process/process_1.h"

extern  t_data data[3];

void  process_2() 
{
	int i,j;
	
	printf("start process_2 \n");
	for (i=0;i<data[0].iteration;i++)
	{
		sleep(data[0].io_time);
		for (j=1;j<data[0].cpu_time;j++);
	}
	printf("end process_2 \n");
	exit(0);
}
