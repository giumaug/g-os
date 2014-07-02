//Linked list stack example

//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>

#include "lib/unistd.h"
#include "general.h"
#include "process/kmalloc_test_proc_2.h"

void push(unsigned int val, STACKNODE **stack);
unsigned int pop(STACKNODE **stack);
int isempty(STACKNODE *stack);
unsigned int top(STACKNODE *stack);
int ii;

void kmalloc_test_proc_2()
{
 int kk=0;
 int i=0;
 char line[1024];
 char *key;
 STACKNODE *stack;
 unsigned int dump[100000];
 unsigned int zz;
 unsigned int xx;	
 unsigned int yy;

 stack = NULL;
 zz=malloc(8);
 //for(;;);
 free(zz);
 for (ii=0;ii<100000;ii++) 
 {
	xx=malloc(8);
	if (xx!=zz) 
	{
		kk++;	
 	}
	free(xx);
 }
//for(;;);

for (i=0;i<100000;i++) push(i, &stack);

 i=0;
 while(!isempty(stack))
 {
  dump[i]=pop(&stack);
  i++;
 }
 
 for(;;);
 return 0;
}

unsigned int top(STACKNODE *stack) {
 return stack->val;
}

void push(unsigned int val, STACKNODE **stack) {
 STACKNODE *newnode;

 newnode = (STACKNODE *)malloc(sizeof(STACKNODE));
 newnode->val =val;
 newnode->next = (*stack);
 (*stack) = newnode;
}

unsigned int pop(STACKNODE **stack) {
 STACKNODE *oldnode;
 unsigned int val;
 char *retval;

 oldnode = (*stack);
 val = (*stack)->val;
 (*stack) = (*stack)->next;
 free(oldnode);
 return val;
}

int isempty(STACKNODE *stack) {
 return stack == NULL;
}
