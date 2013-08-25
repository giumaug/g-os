#include "lib/unistd.h"
#include "lib/stdio.h" 
#include "process/process_1.h"

static int elements[MAXSIZE],maxsize;
void selection(int elements[], int array_size);

/*--------------------------------------------
void process_1()
{
	void *write_buffer;
	void *read_buffer;
	int i;

	write_buffer=malloc(512);
	read_buffer=malloc(512);
	for (i=0;i<512;i++)
 	{
		((char*)write_buffer)[i]=(unsigned char)i;
	}
	//write_sector(100,1,write_buffer);
	read_sector(100,1,read_buffer);
	exit(0);
}
----------------------------------------------*/

/*
void process_1()
{
	int i;
	int pid;

	pid=fork();
	if (pid==0)
	{	
		exec(0x700000,0x100000);
	}
	for (i=0;i<20;i++)
	{
		pid=fork();
		if (pid==0)
		{	
			exec(0x800000,0x100000);
		}
	}		
	exit(0);
}
*/

void process_1()
{
	int i=0;	
	int pid;
	for (i=0;i<10;i++)
	{
		pid=fork();
		if (pid==0)
		{	
			exec(0x700000,0x100000);
		}
	}
	exit(0);
}

/*--------------------------------
	int i;
	for (i=0;i<25;i++) printf("valore=%d \n",&i);
	printf("------- \n");
	for (i=25;i<50;i++) printf("valore=%d \n",&i);
	for(;;);
}
/*--------------------------------------------test video i/o */

/*
	int val=15456;
	printf("starting process_1 \n");
	printf("input value?");
	scanf("%d",&val);
	//printf("\ninput value=%d \n",&val);
	printf("\ninput value=");
	d_printf(val);
	printf("\n");
	printf("end process_1 \n");
	exit(0);
*/

/*----------------------------------------------test video buffer
	int i=0;
        int j=0;
	t_console_desc console_desc;
	init_console(&console_desc,4000,0);
	for (i=0;i<12;i++)
	{
		for (j=0;j<80;j++) 
		{
			write_out_buf(&console_desc,65);
		}
		for (j=0;j<80;j++) 
		{
			write_out_buf(&console_desc,66);
		}
	}
	for (j=0;j<80;j++) 
	{
		write_out_buf(&console_desc,65);
	}
	for (i=0;i<12;i++)
	{
		for (j=0;j<80;j++) 
		{
			write_out_buf(&console_desc,68);
		}
		for (j=0;j<80;j++) 
		{
			write_out_buf(&console_desc,69);
		}
	}
	for (j=0;j<80;j++) 
	{
		write_out_buf(&console_desc,68);
	}
	for (i=0;i<12;i++)
	{
		for (j=0;j<80;j++) 
		{
			write_out_buf(&console_desc,71);
		}
		for (j=0;j<80;j++) 
		{
			write_out_buf(&console_desc,72);
		}
	}
	for (j=0;j<80;j++) 
	{
		write_out_buf(&console_desc,71);
	}
}
*/
//---------------------------------------------------original process_1
//void process_1()
//{
//	int i;
//	for (i=MAXSIZE;i>0;i--) elements[MAXSIZE-i]=i; 
//        maxsize=MAXSIZE;
//	selection(elements, maxsize);
//	while(1);	
//}

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


