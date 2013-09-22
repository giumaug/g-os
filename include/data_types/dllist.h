#ifndef DLLIST_H                
#define DLLIST_H

typedef struct s_llist_node {
  struct s_llist_node *next;
  struct s_llist_node *prev;
  void *val;
} t_llist_node;

typedef struct {
  t_llist_node *sentinel_node;
} t_llist;

extern t_llist *new_dllist();
extern void free_llist(t_llist *l);
extern t_llist_node *ll_prepend(t_llist *l, void *val);
extern t_llist_node *ll_append(t_llist *l, void *val);
extern t_llist_node *ll_insert_before(t_llist_node *n, void *val);
extern t_llist_node *ll_insert_after(t_llist_node *n, void *val);
extern void ll_delete_node(t_llist_node *n);
extern t_llist_node *ll_sentinel(t_llist *l);
extern t_llist_node *ll_first(t_llist *l);
extern t_llist_node *ll_last(t_llist *l);
extern t_llist_node *ll_next(t_llist_node *n);
extern t_llist_node *ll_prev(t_llist_node *n);
extern int ll_empty(t_llist *l);
extern void *ll_val(t_llist_node *n);
extern unsigned int ll_dump(t_llist *l); 

#endif
