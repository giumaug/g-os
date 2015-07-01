#include "memory_manager/kmalloc.h"
#include "data_types/hashtable.h"

static void* hashtable_search(t_hashtable* hashtable,int key,int remove)
{
	int index;
	t_bucket_data* bucket_data;
	t_llist_node* next;
	t_llist_node* sentinel;
	void* value;

	index=key % hashtable->size;
	sentinel=ll_sentinel(hashtable->bucket[index]);
	next=ll_first(hashtable->bucket[index]);
	while (next!=sentinel) 
	{
		bucket_data=next->val;
		if (bucket_data->key==key)
		{
			value=bucket_data->value;
			if (remove)
			{
				ll_delete_node(next);
			}	
			return value;
		}
		next=ll_next(next);
	}
	return NULL;
}

static void hashtable_free_bucket(t_llist** bucket,int size)
{
	int i;

	for (i=0;i<size;i++)
	{
		if ((bucket[i])!=NULL)
		{
			free_llist(bucket[i]);
		}
	}
	kfree(bucket);
}

static void rehash(t_hashtable* hashtable)
{
	int i;	
	t_bucket_data* bucket_data;
	t_llist_node* next;
	t_llist_node* sentinel;
	t_hashtable* new_hashtable;
	t_llist* chained_list;
	
	hashtable_init(new_hashtable,(hashtable->size)*2);
    	for (i=0;i<hashtable->size;i++)
	{
		if (hashtable->bucket[i]!=NULL)
		{
			chained_list=hashtable->bucket[i];
			sentinel=ll_sentinel(chained_list);
			next=ll_first(chained_list);
			while (next!=sentinel) 
			{
				bucket_data=next->val;
				hashtable_put(new_hashtable,bucket_data->key,bucket_data->value);
				next=ll_next(next);
			}
		}
	}
	hashtable_free_bucket(hashtable->bucket,hashtable->size);
	hashtable->bucket=new_hashtable->bucket;
	hashtable->size=new_hashtable->size;
	hashtable->elements=new_hashtable->elements;	
}

void hashtable_init(t_hashtable* hashtable,int init_size)
{
	int i;
	int size;

	size=init_size/LOAD_FACTOR;	
	//hashtable=kmalloc(sizeof(t_hashtable));
	hashtable->bucket=kmalloc(sizeof(t_llist*)*size);
	hashtable->elements=0;
	hashtable->size=size;
	
	for(i=0;i<size;i++) 
	{
		hashtable->bucket[i]=new_dllist();
	}
}

void hashtable_free(t_hashtable* hashtable)
{
	hashtable_free_bucket(hashtable->bucket,hashtable->size);
	kfree(hashtable);
}

void* hashtable_get(t_hashtable* hashtable,int key)
{
	return hashtable_search(hashtable,key,FALSE);
}

void* hashtable_remove(t_hashtable* hashtable,int key)
{
	return hashtable_search(hashtable,key,TRUE);
}

void hashtable_put(t_hashtable* hashtable,int key,void* value)
{
	int index;
	t_bucket_data* bucket_data;
	
	if ((float)(hashtable->elements+1)/(float)hashtable->size>LOAD_FACTOR)
	{
		rehash(hashtable);
	}
	index=key % hashtable->size;
	hashtable->elements++;
	bucket_data=(t_bucket_data*)kmalloc(sizeof(t_bucket_data));
	bucket_data->key=key;
	bucket_data->value=value;
	ll_prepend(hashtable->bucket[index],bucket_data);
}

//djb2 hash function
void hashtable_put_str(t_hashtable* hashtable,char* key,char* value)
{
	unsigned long long_key = 5381;
	int c;

	while (c = *value++)
	{
		long_key = ((long_key << 5) + long_key) + c;
	}
	hashtable_put(hashtable,long_key,value);
}

t_hashtable* hashtable_clone_map(t_hashtable* map)
{
	t_hashtable* cloned_map=NULL;
	void* value=NULL;
	int i=0;

	hashtable_init(cloned_map,map->size);
	for (i=0;i<map->size;i++)
	{
		value=hashtable_get(map,i);
		if (value!=NULL)
		{
			hashtable_put(cloned_map,i,value);
		}
	}
	return cloned_map;
}
