#include "fixed_size.h"
#include <stdio.h>
#include<time.h>

main() 
{
	t_a_fixed_size_desc a_fixed_size_desc;
	unsigned int block_size=512;
	unsigned int init_free_block=100;
	int i=0;
	int x;
	void *a[1000];
	void *b[1000];
	void *c[1000];
	void *p1;
	int counter=0;

	srand(time(NULL));
	a_fixed_size_init(&a_fixed_size_desc,block_size,init_free_block); 
	for (i=0;i<1000;i++)
	{ 
		a[i]=a_fixed_size_alloc(&a_fixed_size_desc);
		//printf("i=%d \n",i);
		//printf("a[i]=%d \n",a[i]);
	}
	a_fixed_size_dump(&a_fixed_size_desc);

	for (i=0;i<500;i++) 
	{
		x = (rand() % 1000);// + 1;
		if (a[x]!=NULL)
		{ 
			printf("x=%d \n",x);
			//printf("a[x]=%d \n",a[x]);
			a_fixed_size_free(&a_fixed_size_desc,a[x]);
			a[x]=NULL;
			counter++;
		}
	}
	a_fixed_size_dump(&a_fixed_size_desc);
	//printf("counter=%d \n",counter);
	for (i=0;i<1000;i++) b[i]=a_fixed_size_alloc(&a_fixed_size_desc);
	for (i=0;i<500;i++) 
	{
		x = (rand() % 1000);// + 1;
		if (b[x]!=NULL)
		{ 
			a_fixed_size_free(&a_fixed_size_desc,b[x]);
			b[x]=NULL;
		}
	}
	a_fixed_size_dump(&a_fixed_size_desc);
	for (i=0;i<1000;i++) c[i]=a_fixed_size_alloc(&a_fixed_size_desc);
	for (i=0;i<500;i++) 
	{
		x = (rand() % 1000);// + 1;
		if (c[x]!=NULL)
		{ 
			a_fixed_size_free(&a_fixed_size_desc,c[x]);
			c[x]=NULL;
		}
	}
	a_fixed_size_dump(&a_fixed_size_desc);
	for (i=0;i<1000;i++) 
	{
		if (a[i]!=NULL) a_fixed_size_free(&a_fixed_size_desc,a[i]);
	}
	a_fixed_size_dump(&a_fixed_size_desc);
	for (i=0;i<1000;i++) 
	{
		if (b[i]!=NULL) a_fixed_size_free(&a_fixed_size_desc,b[i]);
	}
	a_fixed_size_dump(&a_fixed_size_desc);
	for (i=0;i<1000;i++) 
	{
		if (c[i]!=NULL) a_fixed_size_free(&a_fixed_size_desc,c[i]);
	}
	a_fixed_size_dump(&a_fixed_size_desc);
}


