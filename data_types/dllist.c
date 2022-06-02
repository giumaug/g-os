#include "data_types/dllist.h"
#include "data_types/primitive_types.h"

#include "system.h"

t_llist* new_dllist()
{
	dc_new_dllist(NULL);
}

t_llist* dc_new_dllist(void (*data_destructor)(void*))
{
 	t_llist *d;
  	t_llist_node *node;
  
 	d = (t_llist *) kmalloc (sizeof(t_llist));
	d->data_destructor=data_destructor;
 	node = (t_llist_node *)kmalloc(sizeof(t_llist_node));
 	d->sentinel_node = node;
	node->val = NULL;
 	node->next = node;
 	node->prev = node;
 	return d;
}

t_llist_node *ll_insert_before(t_llist_node *node, void *val)
{ 
	if (node > 0xf0000000)
	{
		panic();
	}
	else if (node->prev == NULL)
	{
		panic();
	}


 	t_llist_node *newnode;
 	t_llist_node *prev_node = node->prev;
 	newnode = (t_llist_node *) kmalloc (sizeof(t_llist_node));
 	newnode->val = val;
 	newnode->next = node;
 	newnode->prev = prev_node;
	node->prev = newnode;
	prev_node->next = newnode;
	return newnode;
}

t_llist_node *ll_insert_after(t_llist_node *node, void *val)        
{
	return ll_insert_before(node->next,val);
}

t_llist_node *ll_append(t_llist *l, void *val)
{
	return ll_insert_before(l->sentinel_node,val);
}

t_llist_node *ll_prepend(t_llist *l, void *val)    
{
	return ll_insert_before(l->sentinel_node->next,val);
}

void ll_delete_node(t_llist_node *node)		
{
	t_llist_node *prev_node;
	t_llist_node *next_node;
  
	prev_node=node->prev;
	next_node=node->next;
	prev_node->next=next_node;
	next_node->prev=prev_node;
	//kfree(node->val);
	kfree(node);
}

int ll_empty(t_llist *l)
{
	return (l->sentinel_node->next == l->sentinel_node);
}

//IN THIS CASE REMOVE THE LIST'S CONTENT TOO
//BETTER PROVIDE BOTH POSSIBILITY TO DESTROY AND NOT DESTROY 
//THE CONTENT.EXAMPLE WHAT IF THE SAME OBJECT IS INSIDE TWO DIFFERENT
//LISTS? CHECKING C++ LIBRARY CLEAR EMPTY LIST BUT DOESN'T REMOVE
//OBJECT.IDEALLY WE COULD ALLOW FREE ON EMPTY LIST ONLY AND
//DELETAGE ALL FLUSH JOB TO CALLER.THIS IS IMPRACTICAL BECAUSE LEAD 
//TO LOT OF DUPLICATE CODE.
void free_llist(t_llist *l)
{
	t_llist_node* node;
	
  	while (!ll_empty(l)) 
	{
		node=ll_first(l);
		if (l->data_destructor!=NULL)
		{
			(*l->data_destructor)(node->val);
		}
		else 
		{
			kfree(node->val);
		}
    		ll_delete_node(node);
  	}
  	kfree(l->sentinel_node);
  	kfree(l);
}

void *ll_val(t_llist_node *n)
{
	return n->val;
}

t_llist_node *ll_sentinel(t_llist *l) 
{
	return l->sentinel_node;
}

t_llist_node *ll_first(t_llist *l) 
{
	return l->sentinel_node->next;
}

t_llist_node *ll_last(t_llist *l) 
{
	return l->sentinel_node->prev;
}

t_llist_node *ll_next(t_llist_node *n) 
{
	return n->next;
}

t_llist_node *dll_prev(t_llist_node *n) 
{
	return n->prev;
}

unsigned int ll_size(t_llist *l) 
{
	t_llist_node* next;
	t_llist_node* sentinel;
	int size=0;
	
	sentinel=ll_sentinel(l);
	next=ll_first(l);
	while(next!=sentinel)
	{
		next=ll_next(next);
		size++;
	}
	return size;
}
