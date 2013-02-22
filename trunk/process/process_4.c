#include "lib/unistd.h"
#include "lib/stdlib.h"
#include "process/process_1.h"

extern  t_data data[3];

void  process_4() 
{
	int i,j;

	for (i=0;i<data[2].iteration;i++)
	{
		sleep(data[2].io_time);
		for (j=1;j<data[2].io_time;j++);
	}
	exit(0);
}
