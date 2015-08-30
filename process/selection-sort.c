#include "lib/lib.h"  
#include "selection-sort.h"

#define MAXSIZE 0x100000
//static int elements[MAXSIZE];
static int maxsize;
void selection(int elements[], int array_size);

int main (int argc, char* argv[])
{
	int elements[MAXSIZE];
	int i;
	for (i=MAXSIZE;i>0;i--) 
	{	
		elements[MAXSIZE-i]=i;
	} 
        maxsize=MAXSIZE;
	selection(elements, maxsize);
	for (i=0;i<MAXSIZE;i++) 
	{	
		printf("value=%d \n",elements[i]);
	} 
	exit(0);
}

void selection(int elements[], int array_size)
{
	int i, j, k;
	int min, temp;

	for (i = 0; i < maxsize-1; i++)
	{
		min = i;
		for (j = i+1; j < maxsize; j++)
		{
			if (elements[j] < elements[min])
			min = j;
		}
		temp = elements[i];
		elements[i] = elements[min];
		elements[min] = temp;
	}
}
