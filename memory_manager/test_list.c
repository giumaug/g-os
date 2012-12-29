
#include "fixed_size.h"

main() 
{
	t_a_fixed_size_desc a_fixed_size_desc;
	unsigned int block_size=1024;//2048
	unsigned int init_free_block=10;
	int i=0;
	void *p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8,*p9,*p10,*p11,*p12,*p13,*p14,*p15;


	a_fixed_size_init(&a_fixed_size_desc,block_size,init_free_block); 
	a_fixed_size_dump(&a_fixed_size_desc);
	
	p1=a_fixed_size_alloc(&a_fixed_size_desc);
	p2=a_fixed_size_alloc(&a_fixed_size_desc);
	p3=a_fixed_size_alloc(&a_fixed_size_desc);
	p4=a_fixed_size_alloc(&a_fixed_size_desc);
	p5=a_fixed_size_alloc(&a_fixed_size_desc);
	p6=a_fixed_size_alloc(&a_fixed_size_desc);
	p7=a_fixed_size_alloc(&a_fixed_size_desc);
	p8=a_fixed_size_alloc(&a_fixed_size_desc);
	p9=a_fixed_size_alloc(&a_fixed_size_desc);
	p10=a_fixed_size_alloc(&a_fixed_size_desc);
	p11=a_fixed_size_alloc(&a_fixed_size_desc);
	p12=a_fixed_size_alloc(&a_fixed_size_desc);
	p13=a_fixed_size_alloc(&a_fixed_size_desc);
	p14=a_fixed_size_alloc(&a_fixed_size_desc);
	p15=a_fixed_size_alloc(&a_fixed_size_desc);

	a_fixed_size_dump(&a_fixed_size_desc);

	printf("p1=%d \n",p1);
	printf("p2=%d \n",p2);
	printf("p3=%d \n",p3);
	printf("p4=%d \n",p4);
	printf("p5=%d \n",p5);
	printf("p6=%d \n",p6);
	printf("p7=%d \n",p7);
	printf("p8=%d \n",p8);
	printf("p9=%d \n",p9);
	printf("p10=%d \n",p10);
	printf("p11=%d \n",p11);
	printf("p12=%d \n",p12);
	printf("p13=%d \n",p13);
	printf("p14=%d \n",p14);
	printf("p15=%d \n",p15);
	
	for (i=0;i<=100;i++) *(((char* )p1)+i)=1;
	for (i=0;i<=100;i++) *(((char* )p2)+i)=2;
	for (i=0;i<=100;i++) *(((char* )p3)+i)=3;
	for (i=0;i<=100;i++) *(((char* )p4)+i)=4;
	for (i=0;i<=100;i++) *(((char* )p5)+i)=5;
	for (i=0;i<=100;i++) *(((char* )p6)+i)=6;
	for (i=0;i<=100;i++) *(((char* )p7)+i)=7;
	for (i=0;i<=100;i++) *(((char* )p8)+i)=8;
	for (i=0;i<=100;i++) *(((char* )p9)+i)=9;
	for (i=0;i<=100;i++) *(((char* )p10)+i)=10;

	a_fixed_size_free(&a_fixed_size_desc,p1);
	a_fixed_size_dump(&a_fixed_size_desc);
	a_fixed_size_free(&a_fixed_size_desc,p2);
	a_fixed_size_free(&a_fixed_size_desc,p3);
	a_fixed_size_free(&a_fixed_size_desc,p4);
	a_fixed_size_free(&a_fixed_size_desc,p5);
	a_fixed_size_free(&a_fixed_size_desc,p6);
	a_fixed_size_dump(&a_fixed_size_desc);

	p1=a_fixed_size_alloc(&a_fixed_size_desc);
	p2=a_fixed_size_alloc(&a_fixed_size_desc);
	p3=a_fixed_size_alloc(&a_fixed_size_desc);
	p4=a_fixed_size_alloc(&a_fixed_size_desc);

	printf("p1=%d\n",p1);
	printf("p2=%d\n",p2);
	printf("p3=%d\n",p3);
	printf("p4=%d\n",p4);

	p5=a_fixed_size_alloc(&a_fixed_size_desc);
	p6=a_fixed_size_alloc(&a_fixed_size_desc);

	printf("p5=%d\n",p5);
	printf("p6=%d\n",p6);
	printf("p7=%d\n",p7);
	printf("p8=%d\n",p8);
	printf("p9=%d\n",p9);
	printf("p10=%d\n",p10);
	printf("p11=%d\n",p11);
	printf("p12=%d\n",p12);
	printf("p13=%d\n",p13);
	printf("p14=%d\n",p14);
	printf("p15=%d\n",p15);

	a_fixed_size_free(&a_fixed_size_desc,p1);
	a_fixed_size_dump(&a_fixed_size_desc);
	a_fixed_size_free(&a_fixed_size_desc,p2);
	a_fixed_size_free(&a_fixed_size_desc,p3);
	a_fixed_size_free(&a_fixed_size_desc,p4);
	a_fixed_size_free(&a_fixed_size_desc,p5);
	a_fixed_size_free(&a_fixed_size_desc,p6);
	a_fixed_size_free(&a_fixed_size_desc,p7);
	a_fixed_size_free(&a_fixed_size_desc,p8);
	a_fixed_size_free(&a_fixed_size_desc,p9);
	a_fixed_size_free(&a_fixed_size_desc,p10);
	a_fixed_size_dump(&a_fixed_size_desc);
	a_fixed_size_free(&a_fixed_size_desc,p11);
	a_fixed_size_dump(&a_fixed_size_desc);
	a_fixed_size_free(&a_fixed_size_desc,p12);
	a_fixed_size_free(&a_fixed_size_desc,p13);
	a_fixed_size_dump(&a_fixed_size_desc);
	a_fixed_size_free(&a_fixed_size_desc,p14);
	a_fixed_size_dump(&a_fixed_size_desc);
	a_fixed_size_free(&a_fixed_size_desc,p15);
	a_fixed_size_dump(&a_fixed_size_desc);
}
