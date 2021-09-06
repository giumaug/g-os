#ifndef EXT2_INODE_CACHE_H              
#define EXT2_INODE_CACHE_H

#include "ext2/ext2.h"

struct s_inode;
typedef struct s_element
{
	struct s_inode* inode;
	t_llist_node* list_ref;
	long age;
}
t_element;

typedef struct s_inode_cache
{
	struct s_hashtable* map;
	struct s_llist* list;
	struct s_sem_desc* sem;
}
t_inode_cache;

static t_element* element_init()
{
	t_element* element = NULL;
	
	element = kmalloc(sizeof(t_element));
	return element;
}

static void element_free(t_element* element)
{
	inode_free(element->inode);
	kfree(element);
}

static t_inode_cache* init_inode_cache()
{
	t_inode_cache* inode_cache = NULL;
	struct t_sem_desc* sem_desc = NULL;
	
	inode_cache = kmalloc(sizeof(t_inode_cache));
	inode_cache->map = hashtable_init(INODE_CACHE_SIZE);
    inode_cache->list = dc_new_dllist(&element_free);
	sem_desc = kmalloc(sizeof(t_sem_desc));
	inode_cache->sem = sem_desc;
	sem_init(inode_cache->sem, 1);
	return inode_cache;
}

static void free_inode_cache(t_inode_cache* inode_cache)
{
	hashtable_dispose(inode_cache->map);
	free_llist(inode_cache->list);
	kfree(inode_cache->sem);
	kfree(inode_cache);
}

//For the moment I flush everything, next I will implement a cleaver strategy.
void flush_inode_cache(t_ext2* ext2, u8 lock)
{
	s32 i;
	t_inode* inode = NULL;
	t_element* element = NULL;
	t_llist_node* next = NULL;
	t_llist_node* sentinel = NULL;
	t_inode_cache* inode_cache = NULL;
	t_llist_node** inode_to_free = NULL;
	u32 inode_count;
	s32 index;

	index = -1;
	inode_cache = ext2->superblock->inode_cache;
	inode_count = inode_cache->map->elements;

	if (lock)
	{
		sem_down(ext2->sem);
	}
	inode_to_free = kmalloc(sizeof(t_llist_node*) * inode_count);
	sentinel = ll_sentinel(inode_cache->list);
	next = ll_first(inode_cache->list);
	while (next != sentinel)
	{
		element = (t_element*) next->val;
		inode = element->inode;
		if (inode->counter == 0)
		{
			index++;
			inode_to_free[index] = next;
		}
		next=ll_next(next);
	}

	for (i = 0; i <= index; i++) 
	{
		next = inode_to_free[i];
		element = (t_element*) next->val;
		inode = element->inode;
		hashtable_remove(inode_cache->map, inode->i_number);
		if (inode->status == 1)
		{
			write_inode(ext2, inode, 1, 0);
		}
		ll_delete_node(next);
		inode_prealloc_block_free(inode, 0);
		element_free(element);
	}
	sync_fs(ext2, 0);
	if (lock)
	{
		sem_up(ext2->sem);
	}
	printk("cache hashmap size = %d \n", inode_cache->map->elements);
	printk("cache list size = %d \n", ll_size(inode_cache->list));
}

static void put_inode_cache(t_inode_cache* inode_cache,t_inode* inode)
{
	t_element* element = NULL;

	sem_down(inode_cache->sem);
	element = element_init();
	element->inode = inode;
	element->age = system.time;

	if ((float) (inode_cache->map->elements + 1) / (float) inode_cache->map->size > LOAD_FACTOR) 
	{
		flush_inode_cache(inode->ext2, 0);
	}
	if (hashtable_get(inode_cache->map, inode->i_number != NULL))
	{
		panic();
	}
	hashtable_put(inode_cache->map, inode->i_number, element);
	element->list_ref = ll_append(inode_cache->list, element);
	sem_up(inode_cache->sem);
}

static t_inode* get_inode_cache(t_inode_cache* inode_cache, u32 i_number)
{
	t_inode* inode = NULL;
	t_element* element = NULL;

	sem_down(inode_cache->sem);
	element = hashtable_get(inode_cache->map, i_number);
	if (element != NULL)
	{
		inode = element->inode;
		inode->counter++;
	}
	sem_up(inode_cache->sem);
	return inode;
}

static void remove_inode_cache(t_inode_cache* inode_cache, u32 i_number)
{
	t_inode* inode = NULL;
	t_element* element = NULL;

	sem_down(inode_cache->sem);
	element = hashtable_remove(inode_cache->map, i_number);
	if (hashtable_get(inode_cache->map, i_number) != NULL)
	{
		panic();
	}
	if (element != NULL)
	{
		inode = element->inode;
		ll_delete_node(element->list_ref);
		element_free(element);
	}
	sem_up(inode_cache->sem);
}

#endif
