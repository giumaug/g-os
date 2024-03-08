#include "virtual_memory/vm.h"
#include "memory_manager/general.h"
#include "debug.h"
#include "timer.h"

unsigned int collect_mem=0;
unsigned int collected_mem[50005];
unsigned int collected_mem_index=0;
unsigned int allocated_block=0;
unsigned int start_count = 0;
static int age=0;

void add_tcp_conn(u32 port,t_tcp_conn_desc* conn);
void remove_tcp_conn(u32 port);
void check_tcp_conn();

void _reg_time(long long *timeboard,int* counter)
{
	long long time;

	rdtscl(&time);
	(*counter)++;
	timeboard[*counter] = time;
	return;
}

void check_free_mem()
{
	int last_block;
	int i=0;
	unsigned int buddy_mem;
	unsigned int pool_mem;

	SAVE_IF_STATUS
	CLI
	if (age == 0)
	{
		//collect_mem = 1;
		//start_count = 1;
		//reset_counter();
	}

	buddy_mem=buddy_free_mem(system.buddy_desc);
	pool_mem=kfree_mem();
	//check_tcp_conn();
	//buddy_check_mem_status(system.buddy_desc);
	//a_fixed_size_check_mem_status();

	//check_active_process();
	if (buddy_mem<90700560)
	{
		PRINTK("BUDDY MEMEMORY LEAK!!!");
		//panic();
	}
	if (pool_mem<279000)
	{
		PRINTK("POOL MEMEMORY LEAK!!!");
		//panic();
	}
	//check_leak();
	//check_not_released();
	//_check_process_context();
	printk("BUDDY MEMORY=%d \n",buddy_mem);
	printk("POOL MEMORY=%d \n",pool_mem);
	printk("AGE=%d \n",age++);
	RESTORE_IF_STATUS
}

void _check_process_context_2(int val)
{
	t_llist_node* next;
	t_llist_node* sentinel_node;
	struct t_process_context* process_context;
	int index=0;
	int count = 0;

//	if (system.ops == 0 )
//	{
//		return;
//	}

	while(index<10)
	{
		sentinel_node=ll_sentinel(system.scheduler_desc->scheduler_queue[index]);
		next=ll_first(system.scheduler_desc->scheduler_queue[index]);
		while(next!=sentinel_node)
		{
			process_context=next->val;
			if (process_context->pid == val)
			{
				count++;
			}
			next=next=ll_next(next);
		}
		index++;
	}
	if (count >= 1)
	{
		printk("over 2 \n");
		panic();
	}
}

void __check_process_context(int val)
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
			if (process_context->pid == val)
			{
				printk("index is %d \n",index);
				panic();
			}
			next=next=ll_next(next);
		}
		index++;
	}
}

void _is_phy_page_used(unsigned int phy_page_addr)
{
	u32 i = 0;
	u32 page_addr;

	page_addr = phy_page_addr+BUDDY_START_ADDR + VIRT_MEM_START_ADDR;

	for (i = collected_mem_index; i < 50000;i++)
	{
		if (collected_mem[collected_mem_index] == page_addr)
		{
			panic();
		}
	}
}

void _collect_mem_alloc(unsigned int page_addr)
{
	unsigned int i=0;

	if (collect_mem == 1)
	{
		for (i = collected_mem_index; i < 50000;i++)
		{
			if (collected_mem[collected_mem_index] == page_addr)
			{
				panic();
			}
		}
		for (i = 0; i < collected_mem_index;i++)
		{
			if (collected_mem[collected_mem_index] == page_addr)
			{
				panic();
			}
		}
		collected_mem_index++;
		if (collected_mem_index > 49999)
		{
			collected_mem_index = 0;
			printk("reset counter!!!! \n");
			//panic();
		}
		collected_mem[collected_mem_index] = page_addr;
	}
}

void collect_mem_alloc(unsigned int mem_addr)
{
	int i;
	static int not_init = 0;

	if (not_init == 0)
	{
		not_init = 1;
		for (i = 0; i < 50000; i++)
		{
			collected_mem[i] = 0;
		
		}
	}
	collected_mem_index++;
	if (collected_mem_index > 49999)
	{
		collected_mem_index = 0;
		for (i = 0; i < 50000; i++)
		{
			collected_mem[i] = 0;
		
		}
		printk("reset counter!!!! \n");
		//panic();
	}
	collected_mem[collected_mem_index] = mem_addr;
}

void collect_mem_free(unsigned int page_addr)
{
	int found = 0;
	unsigned int i = 0;

	if (collect_mem == 1)
	{
		for (i = 0; i < 50000; i++)
		{
			if (collected_mem[i] == page_addr)
			{
				collected_mem[i] = 0;
				found = 1;
				break;
			}
		}
		if (found == 0) 
		{
			//panic();
		}
	}
}

void check_leak()
{
	int i;
	int xxx = 0;
	int index;
	for (i = 0;i < 50000;i++)
	{
		if (collected_mem[i] !=0 )
		{
			xxx++;
			index = i;
			printk(" index is %d ",index);
			if (index == 20446)
			{
				printk("mem is %d \n",collected_mem[index]);	
			}
		}
	}
	printk("\n not released count %d \n",xxx);
	printk("index is %d \n",index);
}

void check_open_conn()
{
	int i;
	
	for (i = 0; i <= system.tcp_open_conn_index; i++)
	{
		if (system.tcp_open_conn[i] != 0)
		{
			printk("port open at %d \n", system.tcp_open_conn[i]);
		}
	}
}
