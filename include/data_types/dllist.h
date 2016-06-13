#ifndef DLLIST_H                
#define DLLIST_H

typedef struct s_llist_node 
{
	struct s_llist_node *next;
  	struct s_llist_node *prev;
  	void *val;
} 
t_llist_node;

typedef struct 
{
	t_llist_node *sentinel_node;
	void (*data_destructor)(void*);
} 
t_llist;

t_llist *new_dllist();
t_llist* dc_new_dllist(void (*data_destructor)(void*));
void free_llist(t_llist *l);
t_llist_node *ll_prepend(t_llist *l, void *val);
t_llist_node *ll_append(t_llist *l, void *val);
t_llist_node *ll_insert_before(t_llist_node *n, void *val);
t_llist_node *ll_insert_after(t_llist_node *n, void *val);
void ll_delete_node(t_llist_node *n);
t_llist_node *ll_sentinel(t_llist *l);
t_llist_node *ll_first(t_llist *l);
t_llist_node *ll_last(t_llist *l);
t_llist_node *ll_next(t_llist_node *n);
t_llist_node *ll_prev(t_llist_node *n);
int ll_empty(t_llist *l);
void *ll_val(t_llist_node *n);
unsigned int ll_dump(t_llist *l); 

#endif
