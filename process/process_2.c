#include "lib/stdio.h"
#include "process/process_2.h"

#include "lib/unistd.h"
#include "lib/stdlib.h"

static void quicksort(int low, int high,int* array);

void  process_2() 
{
/*---------------------------------------------
	int pid;
	pid=fork();
	if (pid==0)
	{	
		exec(0x800000,0x100000);
	}
	else 
	{
		exit(0);
	}
	exit(0);
------------------------------------------------------*/
	int array[MAXSIZE];
 	int i = 0;
	printf("starting process_2 \n");
	for (i=MAXSIZE;i>0;i--) array[MAXSIZE-i]=i; 
 	quicksort(0, (MAXSIZE - 1),array);
	//printf("array[0]=%d \n",&array[0]);
	//printf("array[10]=%d \n",&array[10]);
	//printf("array[100]=%d \n",&array[100]);
	//printf("array[1000]=%d \n",&array[1000]);
	printf("array[0]=");
	d_printf(array[0]);
	printf("\n");
	printf("array[10]=");
	d_printf(array[10]);
	printf("\n");
	printf("array[100]=");
	d_printf(array[100]);
	printf("\n");
	printf("array[1000]=");
	d_printf(array[1000]);
	printf("\n");
	printf("end process_2 \n");
	exit(0);
}

static void quicksort(int low, int high,int* array) 
{
 	int i = low;
 	int j = high;
 	int y = 0;

 	int z = array[(low + high) / 2];

 	do 
	{
  		while(array[i] < z) i++;
  		while(array[j] > z) j--;
		if(i <= j) 
		{
   			y = array[i];
   			array[i] = array[j]; 
   			array[j] = y;
   			i++; 
   			j--;
  		}
 	} 
	while(i <= j);
 	if(low < j) 
  		quicksort(low, j,array);

 	if(i < high) 
  		quicksort(i, high,array); 
}

