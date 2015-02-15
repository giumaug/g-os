#define GET_FROM_STACK(n,var) asm("mov %%ebp,%0;":"=r"(var));var+=n+2;
#define STI asm ("sti");
#define CLI asm ("cli");
#define EXIT_SYSCALL_HANDLER asm("pop %ebp;iret;");

#define SAVE_PROCESSOR_REG	asm("push %es");                                      \
				asm("push %ds");                                      \
				asm("push %ebp");                                     \
				asm("push %edi");                                     \
				asm("push %esi");                                     \
				asm("push %edx");                                     \
				asm("push %ecx");                                     \
				asm("push %ebx");                                     \
				asm ("movl %%eax, %0;":"=r"(processor_reg.eax));      \
				asm("pop %ebx");                                      \
				asm ("movl %%ebx, %0;":"=r"(processor_reg.ebx));      \
				asm("pop %ecx");                                      \
				asm ("movl %%ecx, %0;":"=r"(processor_reg.ecx));      \
				asm("pop %edx");                                      \
				asm ("movl %%edx, %0;":"=r"(processor_reg.edx));      \
				asm("pop %esi");                                      \
				asm ("movl %%esi, %0;":"=r"(processor_reg.esi));      \
				asm("pop %edi");                                      \
				asm ("movl %%edi, %0;":"=r"(processor_reg.edi));      \
				asm("pop %ebp");                                      \
				asm ("movl %%ebp, %0;":"=r"(processor_reg.esp));      \
				asm("pop %ds");                                       \
				asm ("movl %%ds, %0;":"=r"(processor_reg.ds));        \
				asm("pop %es");                                       \
				asm ("movl %%es, %0;":"=r"(processor_reg.es));        \


// push and pop registers on stack to avoid inline asm dirties them
#define RESTORE_PROCESSOR_REG	asm ("movl %0,%%esp;"::"r"(_processor_reg.esp)); \
				asm ("movl %0,%%eax;"::"r"(_processor_reg.eax)); \
				asm ("push %eax;");                              \
				asm ("movl %0,%%ebx;"::"r"(_processor_reg.ebx)); \
				asm ("push %ebx;");                              \
				asm ("movl %0,%%ecx;"::"r"(_processor_reg.ecx)); \
				asm ("push %ecx;");                              \
				asm ("movl %0,%%edx;"::"r"(_processor_reg.edx)); \
				asm ("push %edx;");                              \
				asm ("movl %0,%%esi;"::"r"(_processor_reg.esi)); \
				asm ("push %esi;");                              \
				asm ("movl %0,%%edi;"::"r"(_processor_reg.edi)); \
				asm ("push %edi;");                              \
				asm ("movl %0,%%ds;"::"r"(_processor_reg.ds));   \
				asm ("push %ds;");                               \
				asm ("movl %0,%%es;"::"r"(_processor_reg.es));   \
				asm ("push %es;");                               \
				asm ("pop %es");                                 \
				asm ("pop %ds");                                 \
				asm ("pop %edi;");                               \
				asm ("pop %esi;");                               \
				asm ("pop %edx;");                               \
				asm ("pop %ecx;");                               \
				asm ("pop %ebx;");                               \
				asm ("pop %eax;");
                                
#define RET_FROM_INT_HANDLER 	asm("mov %ebp,%esp;pop %ebp;iret");

#define SWITCH_TO_USER_MODE(stack_address)     asm("                               		\
					mov    $0x23,%ax;                         		\
					mov    %ax,%ds;                          		\
        				mov    %ax,%es;                          		\
					push   %eax;                              		\
					movl   $0x23,%eax;	      	   			\
        				push   %eax;                              		\
					");							\
                                        asm("movl %0,%%eax;push %%eax;"::"r"(stack_address)); 	\
					asm("movl   $0x206,%eax;    				\
					push   %eax;                              		\
        				movl   $0x13,%eax;      /*cs*/      	   		\
					push   %eax;                              		\
        				movl   $0x100000,%eax;  /*eip*/           		\
					push   %eax;                                            \
					iret;");

//#define	SWITCH_DS_TO_KERNEL_MODE asm("                                             \
//				      	.comm TMP,2;                               \
//        			      	mov  %ax,TMP; 				   \
//				      	mov $0x18,%ax;                             \
//				      	mov  %ax,%ds;				   \
//				      	mov %ax,%es;				   \
//				      	mov TMP,%ax;	                           \
//				     ");
//
//#define	SWITCH_DS_TO_USER_MODE asm("						   \
//				      	.comm TMP_2,2;                             \
//        			      	mov  %ax,TMP; 				   \
//				      	mov $0x20,%ax;                             \
//				      	mov  %ax,%ds;				   \
//				      	mov %ax,%es;				   \
//				      	mov TMP,%ax;			           \
//				     ");
//
//#define	GET_DS(ds)  asm("						           \
//        			      	mov  %ax,TMP; 				   \
//				      	mov  %ds,%ax;                              \
//                    ");                                                            \
//		asm ("                  mov  %%ax, %0;":"=r"(ds));                 \
//                asm ("			mov TMP,%ax;                               \
//		    ");

#define	SWITCH_DS_TO_KERNEL_MODE asm("                                             \
        			      	push %ax; 				   \
				      	mov $0x18,%ax;                             \
				      	mov  %ax,%ds;				   \
				      	mov %ax,%es;				   \
				      	pop %ax;	                           \
				     ");

#define	SWITCH_DS_TO_USER_MODE asm("						   \
        			      	push  %ax; 				   \
				      	mov $0x20,%ax;                             \
				      	mov  %ax,%ds;				   \
				      	mov %ax,%es;				   \
				      	pop %ax;			           \
				     ");

#define	GET_DS(ds)  asm("						           \
        			      	push %ax; 				   \
				      	mov  %ds,%ax;                              \
                    ");                                                            \
		asm ("                  mov  %%ax, %0;":"=r"(ds));                 \
                asm ("			pop %ax;                                   \
		    ");	

#define GET_STACK_POINTER(ustack_pointer)	asm("                                            \
						     push %%eax;                                 \
						     mov %%esp,%%eax;                            \
						     mov  %%eax, %0;                             \
						     pop %%eax;":"=r"(ustack_pointer));

#define GET_FAULT_ADDRESS(fault_addr,fault_code) asm ("						 \
							push %%eax;				 \
							mov %%cr2,%%eax;		         \
							mov  %%eax, %0;	                         \
							mov  4(%%ebp), %1;                       \
							pop %%eax;"                              \
							:"=r"(fault_addr),"=r"(fault_code)       \
						      );             

#define EOI_TO_MASTER_PIC asm("mov $0x20,%%al; \
		 out %%al,$0x20;"              \
		:                              \
		:                              \
		:"%eax");

#define EOI_TO_SLAVE_PIC asm("mov $0x20,%%al;  \
		 out %%al,$0xa0;"              \
		:                              \
		:                              \
		:"%eax");

#define SAVE_IF_STATUS          unsigned int if_status;          \
	asm("                   push %eax;                       \
				pushfl;                          \
				movl (%esp),%eax;                \
				andl $0x200,%eax;                \
	");	                                                 \
        asm("                   movl %%eax,%0;":"=r"(if_status));\
	asm("	                popfl;                           \
				pop %eax;                        \
        ");

#define RESTORE_IF_STATUS asm("                                  \
				push %eax;                       \
				pushfl;                          \
			      ");	                         \
		          asm(" movl %0,%%eax;"::"r"(if_status));\
		          asm("	orl %eax,(%esp);	         \
				popfl;			         \
				pop %eax;                        \
			  ");	
 
#define HALT asm("sti;hlt");

#define SWITCH_PAGE_DIR(page_dir) asm("mov %0,%%eax;mov %%eax,%%cr3;"::"r"(page_dir):"%eax");
#define SYSCALL(syscall_num,params) asm("mov %0,%%eax;mov %1,%%ecx;int $0x80"::"r"(syscall_num),"r"(params):"%eax","%ecx");
#define SUSPEND asm("mov $0x65,%%eax;int $0x80":::"%eax","%ecx");
#define DO_STACK_FRAME(esp) asm("movl %0,%%esp;"::"r"(esp));

unsigned char in(unsigned int address);
void out(unsigned char value,unsigned int address);
unsigned short inw(unsigned int address);
void outw(unsigned short value,unsigned int address);
void switch_to_user_mode();
void adjust_kernel_stack(unsigned int,unsigned int);
void syscall(unsigned int syscall_num,int* params);
