#include "lib/stdio.h"
#include "process/process_3.h"

//static int list[MAXSIZE];
void swap(int *x,int *y);
void bublesort(int n,int* list);
	
void process_3()
{
	int list[MAXSIZE];
	int i = 0;
	printf("starting process_3 \n");
	for (i=MAXSIZE;i>0;i--) list[MAXSIZE-i]=i; 
	bublesort(MAXSIZE,list);
	//printf("list[0]=%d \n",&list[0]);
	//printf("list[10]=%d \n",&list[10]);
	//printf("list[100]=%d \n",&list[100]);
	//printf("list[1000]=%d \n",&list[1000]);
	//printf("end process_3 \n");
//	printf("list[0]=");
//	d_printf(list[0]);
//	printf("\n");
//	printf("list[10]=");
//	d_printf(list[10]);
//	printf("\n");
//	printf("list[100]=");
//	d_printf(list[100]);
//	printf("\n");
//	printf("list[1000]=");
//	d_printf(list[1000]);
//	printf("\n");
	printf("end process_3 \n");
	exit(0);
}
 
void swap(int *x,int *y)
{
	int temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

void bublesort(int n,int* list)
{
	int i,j;
	for(i=0;i<(n-1);i++)
		for(j=0;j<(n-(i+1));j++)
	             if(list[j] > list[j+1])
	                    swap(&list[j],&list[j+1]);
}
