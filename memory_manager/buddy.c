#include "general.h"
#include "asm.h"
#include "synchro_types/spin_lock.h"
#include "memory_manager/general.h"
#include "virtual_memory/vm.h"
#include "memory_manager/buddy.h"

static unsigned int mem;

void buddy_init(t_buddy_desc* buddy)
{	
	unsigned int mem_addr;
	unsigned int num_entry;
	void *page_addr;
	unsigned int max_page_size;
	unsigned int align_offset;
	unsigned int* mem_addr_bucket;
	int i,j;
	
	mem_addr=0;
	max_page_size=PAGE_SIZE*(1<<(NUM_LIST-1));
	
	for (i=0;i<NUM_LIST;i++) 
	{
		buddy->page_list[i]=new_dllist();
	}
	num_entry=BUDDY_MEM_SIZE/max_page_size;
	for (j=0;j<num_entry;j++) 
	{	
		buddy->order[BLOCK_INDEX(mem_addr)]=NUM_LIST-1;
		mem_addr_bucket=kmalloc(sizeof(unsigned int));
		*mem_addr_bucket=mem_addr;			
		ll_prepend(buddy->page_list[NUM_LIST-1],mem_addr_bucket);
		buddy->page_list_ref[BLOCK_INDEX(mem_addr)]=0;
		mem_addr+=max_page_size;
	}
	return;
}
	
void* buddy_alloc_page(t_buddy_desc* buddy,unsigned int mem_size)
{
	void* page_addr;	
	t_llist_node* node;
	t_llist_node* node_buddy;
	unsigned int next_list_index;
	unsigned int list_index=1;
	unsigned int list_found=0;
	unsigned int page_size;
	unsigned int* mem_addr_bucket;
	void* new_mem_addr;
	int i;

	SAVE_IF_STATUS
	CLI	
	SPINLOCK_LOCK	
	for (list_index=0;list_index<NUM_LIST;list_index++) 
	{	
		if (PAGE_SIZE*(1<<list_index)>=mem_size) break;
	}
	next_list_index=list_index;
	while (next_list_index<NUM_LIST && !list_found)
	{
		if (!ll_empty(buddy->page_list[next_list_index]))
		{
        		node=ll_first(buddy->page_list[next_list_index]);
			list_found=1;
		}
		else 
		{
			next_list_index++;
		}
	}
	page_addr=*(unsigned int*)(node->val);
	kfree(node->val);
	ll_delete_node(node);
	if (next_list_index!=list_index)
	{
		page_size=PAGE_SIZE*(1<<next_list_index);
                for(i=(next_list_index-1);i>=(int)list_index;i--)
		{
			page_size/=2;
			mem_addr_bucket=kmalloc(sizeof(unsigned int));
			*mem_addr_bucket=page_addr+page_size;
			node_buddy=ll_prepend(buddy->page_list[i],mem_addr_bucket);
			buddy->order[BLOCK_INDEX((int)page_addr+page_size)]=i;
			buddy->page_list_ref[BLOCK_INDEX((int)page_addr+page_size)]=node_buddy;
		}
	}
	buddy->order[BLOCK_INDEX((int)page_addr)]=(list_index | 16);
	buddy->page_list_ref[BLOCK_INDEX((int)page_addr)]=node;
	new_mem_addr=page_addr+BUDDY_START_ADDR + VIRT_MEM_START_ADDR;
	SPINLOCK_UNLOCK
	RESTORE_IF_STATUS
	return new_mem_addr;
}

void buddy_free_page(t_buddy_desc* buddy,void* to_free_page_addr)
{
	unsigned int stop_coalesce;	
	unsigned int list_index;
	unsigned int page_size;
	unsigned int near_index;
        unsigned int page_addr;
	unsigned int buddy_page_addr; 
	unsigned int free_page_addr;
	unsigned int free_page_order;
	unsigned int* mem_addr_bucket;
	t_llist_node* node;
	t_llist_node* node_buddy;
	int i;

	SAVE_IF_STATUS
	CLI	
	SPINLOCK_LOCK
	page_addr=to_free_page_addr;
	page_addr-=(BUDDY_START_ADDR + VIRT_MEM_START_ADDR);
	
	if ((buddy->order[BLOCK_INDEX(page_addr)] & 16)==0)
	{
		return;
	}
	
	list_index=buddy->order[BLOCK_INDEX(page_addr)] & 15;
	free_page_addr=page_addr;
	free_page_order=list_index;
	stop_coalesce=0;	
	i=list_index;
	while(i<NUM_LIST-1 && !stop_coalesce)
	{
		page_size=PAGE_SIZE*(1<<i);
                buddy_page_addr=page_addr^page_size;
		if ((buddy->order[BLOCK_INDEX(buddy_page_addr)] & 15)==i && 
		   (buddy->order[BLOCK_INDEX(buddy_page_addr)] & 16)==0 && 
		    buddy->page_list_ref[BLOCK_INDEX((buddy_page_addr))]!=NULL)
		{
			node=buddy->page_list_ref[BLOCK_INDEX((buddy_page_addr))];
			kfree(node->val);			
			ll_delete_node(node);
			buddy->order[BLOCK_INDEX(buddy_page_addr)]=0;
			buddy->page_list_ref[BLOCK_INDEX((buddy_page_addr))]=NULL;
			page_addr&=buddy_page_addr;
			free_page_addr=page_addr;
			free_page_order=i+1;
			page_addr&=buddy_page_addr;
			buddy->order[BLOCK_INDEX(page_addr)]= buddy->order[BLOCK_INDEX(page_addr)] & 15;
		}
		else 
		{
			stop_coalesce=1;
		}
		i++;
	}
	mem_addr_bucket=kmalloc(sizeof(unsigned int));
	*mem_addr_bucket=free_page_addr;			
	node_buddy=ll_prepend(buddy->page_list[free_page_order],mem_addr_bucket);
	buddy->page_list_ref[BLOCK_INDEX(free_page_addr)]=node_buddy;
	buddy->order[BLOCK_INDEX(free_page_addr)]=free_page_order;
	SPINLOCK_UNLOCK
	RESTORE_IF_STATUS
}

//void buddy_dump_mem(t_buddy* buddy)
//{
//	t_llist* list;
//	t_llist_node* next;
//	t_llist_node* sentinel;
//	unsigned int* val;
//	unsigned int i;
//	unsigned int counter;
//
//	for (i=0;i<NUM_LIST;i++)
//	{
//		printf("--------------------------list %d --------------------------------\n",i);
//		counter=1;		
//		list=buddy->page_list[i];
//		sentinel=ll_sentinel(list);
//		next=ll_first(list);
//		while(next!=sentinel)
//		{
//			val=next->val;
//			printf("list entry=%d value=%x \n",counter,*val);
//			counter++;
//			next=ll_next(next);
//		}
//		printf("------------------------------------------------------------------\n");
//	}
//}