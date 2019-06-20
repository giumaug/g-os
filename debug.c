#include "virtual_memory/vm.h"
#include "memory_manager/general.h"
#include "debug.h"
#include "timer.h"

extern go;
extern unsigned int free_mem_list[POOL_NUM];

unsigned int pending_port[10000];
unsigned int pending_port_age[10000];
t_tcp_conn_desc* pending_connection[10000];
unsigned int pending_port_index;
unsigned int trace_buffer_1[10000];
unsigned int trace_buffer_2[10000];
unsigned int trace_buffer_3[10000];
unsigned int trace_buffer_4[10000];
int indx_2=0;
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
		reset_counter();
	}

	buddy_mem=buddy_free_mem(system.buddy_desc);
	pool_mem=kfree_mem();
	check_tcp_conn();
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
	check_not_released();
	_check_process_context();
	printk("BUDDY MEMORY=%d \n",buddy_mem);
	printk("POOL MEMORY=%d \n",pool_mem);
	printk("AGE=%d \n",age++);
	RESTORE_IF_STATUS
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
			{
				count++;
				if (system.fork > 2 && process_context->proc_status == SLEEPING)
				{
					//panic();
				}
			}
			next=next=ll_next(next);
		}
		index++;
	}
	if (count > 7 )
	{
		panic();
	}
}

void _is_phy_page_used(unsigned int phy_page_addr)
{
	u32 i = 0;
	u32 page_addr;

	page_addr = phy_page_addr+BUDDY_START_ADDR + VIRT_MEM_START_ADDR;

	for (i = collected_mem_index; i < 5000;i++)
	{
		if (collected_mem[collected_mem_index] == page_addr)
		{
			panic();
		}
	}
}

void collect_mem_alloc(unsigned int page_addr)
{
	unsigned int i=0;

	if (collect_mem == 1)
	{
		for (i = collected_mem_index; i < 5000;i++)
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
		if (collected_mem_index > 4999)
		{
			collected_mem_index = 0;
			printk("reset counter!!!! \n");
			//panic();
		}
		collected_mem[collected_mem_index] = page_addr;
	}
}

void collect_mem_free(unsigned int page_addr)
{
	int found=0;
	unsigned int i=0;

	if (collect_mem == 1)
	{
		for (i = 0;i < 5000;i++)
		{
			if (collected_mem[i] == page_addr)
			{
				collected_mem[i] = 0;
				found=1;
				break;
			}
		}
		if (found==0) 
		{
			found=1;
			panic();
		}
	}
}

void check_not_released()
{
	int i = 0;
	int index = 0;
	
	printk("fork = %d \n",system.fork);
	printk("out = %d \n",system.out);
	printk("piggy = %d \n",system.piggy_timeout);
	printk("rtrsn = %d \n",system.rtrsn_timeout);
	printk("reset = %d \n",system.reset);
}

void reset_counter()
{
	int i;
	collected_mem_index = 0;
	for (i = 0; i <50000;i++)
	{
		collected_mem[i] = 0;
	}
	reset_tcp_counter();
}

void reset_tcp_counter()
{
	int i;

	for (i = 0; i <10000;i++)
	{
		pending_port[i] = 0;
		pending_port_age[i] = 0;
		pending_connection[i] = 0;
	}
	pending_port_index = 0;
}

void add_tcp_conn(u32 port,t_tcp_conn_desc* conn)
{
	int i;

	pending_port_index++;
	if (pending_port_index == 10000)
	{
		reset_tcp_counter();
	}
	pending_port[pending_port_index] = port;
	pending_port_age[pending_port_index] = (system.tcp_1 % 10000);
	pending_connection[pending_port_index] = conn;
}
void remove_tcp_conn(u32 port)
{
	int i;

	for (i = 0; i <10000;i++)
	{
		if (pending_port[i] == port)
		{
			pending_port[i] = 0;
			pending_port_age[i] = 0;
			pending_connection[i] = 0;
			continue;
		}
	}
}

void check_tcp_conn()
{
	u32 counter;
	int i;
	
	counter = system.tcp_1 % 10000;	

	if (counter < pending_port_index)
	{
		return;
	}

	for (i = 0; i <10000;i++)
	{
		if (pending_port[i] != 0 && (counter - pending_port_age[i]) > 8000 )
		{
			panic();
		}
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
	trace_buffer_4[trace_index] = system.time;
}

//USEFUL CODE TO CHECK STACK MEMORY CORRUPTION
//if (_new_process_context.pid > 2 && _new_process_context.pending_fork == 99)
//{
//	((struct t_process_context*)(system.process_info->current_process->val))->pending_fork = 0;
//	if (*(int*)(_new_process_context.processor_reg.esp+4) != TEST_STACK)
//	{
//		panic();
//	}
//	if (**_tmp != TEST_USER_SPACE)
//	{
//		panic();
//	}
//	
//	_tmp2 = _tmp + 3;
//	_tmp3 = (*_tmp2) + 24;
//	if (*_tmp3 != AFTER_FORK)
//	{
//		page_table_new = ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*) _new_process_context.page_dir)[767]));
//		phy_fault_addr_new = ALIGN_4K(((unsigned int*) page_table_new)[1019]); 
//		if (phy_fault_addr_new == 0)
//		{
//			printk("!!\n");
//		}
//		page_table_old = ALIGN_4K(FROM_PHY_TO_VIRT(((unsigned int*) _old_process_context.page_dir)[767]));
//		phy_fault_addr_old = ALIGN_4K(((unsigned int*) page_table_old)[1019]); 
//		if (phy_fault_addr_old == 0)
//		{
//			printk("!!\n");
//		}
//		panic();
//	}
//}

#define MAXSIZE 0x3E8
static int maxsize;
int sort()
{
	int elements[MAXSIZE];
	int i;
	for (i=MAXSIZE;i>0;i--) 
	{	
		elements[MAXSIZE-i]=i;
	} 
        maxsize=MAXSIZE;
	selection(elements, maxsize);
}

void selection(int elements[], int array_size)
{
	int i, j, k;
	int min, temp;
	int lll = 0;

	for (i = 0; i < maxsize-1; i++)
	{
		lll++;
		min = i;
		for (j = i+1; j < maxsize; j++)
		{
			if (elements[j] < elements[min])
			min = j;
			lll++;
		}
		temp = elements[i];
		elements[i] = elements[min];
		elements[min] = temp;
	}
}
