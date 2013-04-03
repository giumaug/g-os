#ifndef HASHTABLE_H                
#define HASHTABLE_H
#include "dllist.h"

#define LOAD_FACTOR .75

struct s_hashtable
{ 	
	int size;
	int elements;
	t_llist** bucket;
}
typedef t_hashtable;

struct s_bucket_data
{
	int key;
	void* value;
}
typedef t_bucket_data;

t_hashtable* hashtable_init(int init_size);
void hashtable_free(t_hashtable* hashtable);
void* hashtable_get(t_hashtable* hashtable,int key);
void hashtable_put(t_hashtable* hashtable,int key,void* value);

#endif

