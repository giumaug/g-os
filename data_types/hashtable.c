#include "lib/lib.h"
#include "memory_manager/kmalloc.h"
#include "data_types/hashtable.h"

static void* hashtable_search(t_hashtable* hashtable,u32 key,int remove)
{
	u32 index;
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
				kfree(bucket_data);
				hashtable->elements--;
			}	
			return value;
		}
		next=ll_next(next);
	}
	return NULL;
}

static void hashtable_free_bucket(t_llist** bucket,u32 size,void (*data_destructor)(void*),int remove)
{
	u32 i;
	t_bucket_data* bucket_data;
	t_llist_node* next;
	
	for (i=0;i<size;i++)
	{
		if ((bucket[i])!=NULL)
		{
			while (!ll_empty(bucket[i])) 
			{
				next=ll_first(bucket[i]);
				bucket_data=next->val;
				if (remove == TRUE)
				{
					if (data_destructor!=NULL)
					{
						(*data_destructor)(bucket_data->value);
					}
					else 
					{
						kfree(bucket_data->value);
					}
				}
				kfree(bucket_data);
    				ll_delete_node(next);
  			}
		}
		free_llist(bucket[i]);
	}
	kfree(bucket);
}

static void rehash(t_hashtable* hashtable)
{
	u32 i;	
	t_bucket_data* bucket_data;
	t_llist_node* next;
	t_llist_node* sentinel;
	t_hashtable* new_hashtable;
	t_llist* chained_list;
	
	new_hashtable=hashtable_init((hashtable->size)*2);
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
	hashtable_free_bucket(hashtable->bucket,hashtable->size,hashtable->data_destructor,FALSE);
	hashtable->bucket=new_hashtable->bucket;
	hashtable->size=new_hashtable->size;
	hashtable->elements=new_hashtable->elements;	
}

t_hashtable* hashtable_init(u32 init_size)
{
	dc_hashtable_init(init_size,NULL);
}

t_hashtable* dc_hashtable_init(u32 init_size,void (*data_destructor)(void*))
{
	u32 i;
	t_hashtable* hashtable; 

	hashtable=kmalloc(sizeof(t_hashtable));
	hashtable->bucket=kmalloc(sizeof(t_llist*)*init_size);
	hashtable->elements=0;
	hashtable->size=init_size;
	hashtable->data_destructor=data_destructor;
	
	for(i=0;i<init_size;i++) 
	{
		hashtable->bucket[i]=new_dllist();
	}
	return hashtable;
}

void hashtable_free(t_hashtable* hashtable)
{
	hashtable_free_bucket(hashtable->bucket,hashtable->size,hashtable->data_destructor,TRUE);
	kfree(hashtable);
}

void hashtable_dispose(t_hashtable* hashtable)
{
	hashtable_free_bucket(hashtable->bucket,hashtable->size,hashtable->data_destructor,FALSE);
	kfree(hashtable);
}

void* hashtable_get(t_hashtable* hashtable,u32 key)
{
	return hashtable_search(hashtable,key,FALSE);
}

void* hashtable_remove(t_hashtable* hashtable,u32 key)
{
	return hashtable_search(hashtable,key,TRUE);
}

void hashtable_put(t_hashtable* hashtable,u32 key,void* value)
{
	u32 index;
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
void hashtable_put_str(t_hashtable* hashtable,unsigned char* key,char* value)
{
	unsigned long long_key = 5381;
	u32 c;

	while (c = *value++)
	{
		long_key = ((long_key << 5) + long_key) + c;
	}
	hashtable_put(hashtable,long_key,value);
}

t_hashtable* hashtable_clone_map(t_hashtable* map,u32 data_size)
{
	t_hashtable* cloned_map=NULL;
	void* value=NULL;
	void* cloned_value=NULL;
	u32 i=0;

	cloned_map=hashtable_init(map->size);
	for (i=0;i<map->size;i++)
	{
		value=hashtable_get(map,i);
		if (value!=NULL)
		{
			cloned_value=kmalloc(data_size);
			kmemcpy(cloned_value,value,data_size);
			hashtable_put(cloned_map,i,cloned_value);
		}
	}
	return cloned_map;
}

void _dump_hashtable(t_hashtable* hashtable)
{
	u32 i;	
	t_bucket_data* bucket_data;
	t_llist_node* next;
	t_llist_node* sentinel;
	t_hashtable* new_hashtable;
	t_llist* chained_list;
	
	printk("---------------------------start %d \n",hashtable);
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
				printk("val is %d \n",bucket_data->key);
				printk("data is %d \n",bucket_data->value);
				next=ll_next(next);
			}
		}
	}
	printk("---------------------------end \n");
}
