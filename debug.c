#include "virtual_memory/vm.h"
#include "memory_manager/general.h"
#include "debug.h"

extern go;
extern unsigned int free_mem_list[POOL_NUM];

unsigned int trace_buffer_1[10000];
unsigned int trace_buffer_2[10000];
unsigned int trace_buffer_3[10000];
int index_2=0;
unsigned int collect_mem=0;
unsigned int collected_mem[500];
unsigned int collected_mem_index=0;
unsigned int allocated_block=0;
unsigned int start_count = 0;
long x;
void check_free_mem()
{
	static int age=0;
	int last_block;
	int i=0;
	unsigned int buddy_mem;
	unsigned int pool_mem;

	if (age == 0)
	{
		//collect_mem = 1;
		//start_count = 1;
		//reset_counter();
	}

	buddy_mem=buddy_free_mem(system.buddy_desc);
	pool_mem=kfree_mem();
	//buddy_check_mem_status(system.buddy_desc);
	//a_fixed_size_check_mem_status();

	//check_active_process();
	if (buddy_mem<160700560)
	{
		PRINTK("BUDDY MEMEMORY LEAK!!!");
		//panic();
	}
	if (pool_mem<279000)
	{
		PRINTK("POOL MEMEMORY LEAK!!!");
		//panic();
	}
	check_not_released();
	printk("BUDDY MEMORY=%d \n",buddy_mem);
	printk("POOL MEMORY=%d \n",pool_mem);
	printk("AGE=%d \n",age++);
	//collected_mem_index=0;
	//allocated_block=0;
}

void check_process_context()
{
	return;
}

void _check_process_context()
{
	t_llist_node* next;
	t_llist_node* sentinel_node;
	struct t_process_context* process_context;
	int index=0;
	int count = 0;

	while(index<10)
	{
		sentinel_node=ll_sentinel(system.scheduler_desc->scheduler_queue[index]);
		next=ll_first(system.scheduler_desc->scheduler_queue[index]);
		while(next!=sentinel_node)
		{
			process_context=next->val;
			//if (process_context->curr_sched_queue_index>9 || process_context->curr_sched_queue_index<0)
			if (process_context->pid==2 && go==1)
			{
				count++;
			}
			next=next=ll_next(next);
		}
		index++;
	}
}

//void check_active_process()
//{
//	t_llist_node* next;
//	t_llist_node* sentinel_node;
//	struct t_process_context* process_context;
//	int index=0;
//	
//	index_2=0;
//
//
//	while(index<10)
//	{
//		sentinel_node=ll_sentinel(system.scheduler_desc->scheduler_queue[index]);
//		next=ll_first(system.scheduler_desc->scheduler_queue[index]);
//		while(next!=sentinel_node)
//		{
//			process_context=next->val;
//			proc[index_2]=process_context->pid;
//			index_2++;
//			next=next=ll_next(next);
//		}
//		index++;
//	}
//}

void collect_mem_alloc(unsigned int page_addr)
{
	if (collect_mem == 1 && start_count == 1)
	{
		collected_mem[collected_mem_index++]=page_addr;
		if (collected_mem_index>49999)
		{
			collected_mem_index = 0;
			//panic();
		}
	}
}

void collect_mem_free(unsigned int page_addr)
{
	int found=0;
	unsigned int i=0;
	
	if (collect_mem == 1 && start_count ==1 )
	{
		for (i=0;i<=50000;i++)
		{
			if (collected_mem[i]==page_addr)
			{
				allocated_block--;
				collected_mem[i]=0;
				found=1;
				break;
			}
		}
		if (found==0) 
		{
			found=1;
			//printk("no!! \n");
			//panic();
		}
	}
}

void check_not_released()
{
	int i = 0;
	int index = 0;
	//if (start_count == 1)
	//{	
	//	//for (i=(collected_mem_index - 2000);i<=(collected_mem_index - 1000);i++)
	//	for (i = 0;i <= collected_mem_index;i++)
	//	{
	//		if (collected_mem[i] != 0)
	//		{
	//			index++;
	//		}
	//	}
	//}
	//if (index >0 ) 
	//{
		//printk("not releaed=%d \n",index);
		//printk("collected_mem_index= %d \n",collected_mem_index);
		//printk("tcp_1 = %d \n",system.tcp_1);
		//printk("tcp_1 = %d \n",system.tcp_2);
		printk("fork = %d \n",system.fork);
		printk("exit_0 = %d \n",system.exit_0);
		//printk("exit_1 = %d \n",system.exit_1);
		printk("free_vm = %d \n",system.free_vm);
		printk("out = %d \n",system.out);
		//printk("action2 = %d \n",system.action2);
		printk("trig = %d \n",system.trig);
		printk("cpanic = %d \n",system.cpanic);
	//	panic();
	//}
}

void reset_counter()
{
	int i;
	collected_mem_index = 0;
	for (i = 0; i <50000;i++)
	{
		collected_mem[i] = 0;
	}
}

void trace(int pid,int trace_code,int call_num)
{
	unsigned int static trace_index = 0;
	trace_index++;
	if (trace_index == 10000)
	{
		trace_index = 0;
	}
	trace_buffer_1[trace_index] = trace_code;
	trace_buffer_2[trace_index] = pid;
	trace_buffer_3[trace_index] = call_num;	
}





