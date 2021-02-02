#ifndef EXT2_INODE_CACHE_H              
#define EXT2_INODE_CACHE_H

#include "ext2/ext2.h"

struct s_inode;
typedef struct s_element
{
	struct s_inode* inode;
	long age;
}
t_element;

typedef struct s_inode_cache
{
	struct s_hashtable* map;
	struct s_llist* list;
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
}

static t_inode_cache* init_inode_cache()
{
	t_inode_cache* inode_cache = NULL;
	
	inode_cache = kmalloc(sizeof(t_inode_cache));
	inode_cache->map = hashtable_init(INODE_CACHE_SIZE);
    inode_cache->list = dc_new_dllist(&element_free);
	return inode_cache;
}

static void free_inode_cache(t_inode_cache* inode_cache)
{
	hashtable_dispose(inode_cache->map);
	free_llist(inode_cache->list);
}

//For the moment I flush everything, next I will implement a cleaver strategy.
static void flush_inode_cache(t_ext2* ext2, t_inode_cache* inode_cache)
{
	t_inode* inode = NULL;
	t_element* element = NULL;
	t_llist_node* next = NULL;
	t_llist_node* sentinel_node = NULL;

	sentinel_node=ll_sentinel(inode_cache->list);
	next=ll_first(inode_cache->list);
	while(sentinel_node != next)
	{
		ll_delete_node(next);
		element = (t_element*) next->val;
		inode = element->inode;
		hashtable_remove(inode_cache->map, inode->i_number);
		write_inode(ext2, inode, 1, 1);
		inode_free(inode);
		next = ll_first(inode_cache->list);
	}
}

static void put_inode_cache(t_inode_cache* inode_cache,t_inode* inode)
{
	t_element* element = NULL;

	element = element_init();
	element->inode = inode;
	element->age = system.time;
	
	if (inode_cache->map->elements < INODE_CACHE_SIZE)
	{
		hashtable_put(inode_cache->map, inode->i_number, element);
		ll_append(inode_cache->list, element);
	}
}

static t_inode* get_inode_cache(t_inode_cache* inode_cache, u32 i_number)
{
	t_inode* inode = NULL;
	t_element* element = NULL;

	element = hashtable_get(inode_cache->map, inode->i_number);
	inode = element->inode;
	return inode;
}

#endif
