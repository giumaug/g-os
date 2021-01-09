#ifndef HASHTABLE_H                
#define HASHTABLE_H

#include "data_types/dllist.h"
#include "data_types/primitive_types.h"


#define LOAD_FACTOR .75

struct s_hashtable
{ 	
	int size;
	int elements;
	t_llist** bucket;
	void (*data_destructor)(void*);
}
typedef t_hashtable;

struct s_bucket_data
{
	int key;
	void* value;
}
typedef t_bucket_data;

t_hashtable* hashtable_init(u32 init_size);
t_hashtable* dc_hashtable_init(u32 init_size,void (*data_destructor)(void*));
void hashtable_free(t_hashtable* hashtable);
void hashtable_dispose(t_hashtable* hashtable);
void* hashtable_get(t_hashtable* hashtable,u32 key);
void* hashtable_remove(t_hashtable* hashtable,u32 key);
void hashtable_put(t_hashtable* hashtable,u32 key,void* value);
void hashtable_put_str(t_hashtable* hashtable,unsigned char* key,char* value);
t_hashtable* hashtable_clone_map(t_hashtable* map,u32 data_size);

#endif


