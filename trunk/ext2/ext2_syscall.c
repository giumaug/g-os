#include "data_types/hashtable.h"
#include "ext2/ext2.h"

int open(const char *pathname, int flags, mode_t mode); 
{
	//se file esiste carico inode da disco 
	//se file non esiste alloco inode
	u32 fd;
	struct t_process_context* current_process_context;
	t_inode* inode;
	t_llist_node* node;

	fd=current_process_context->next_fd++;
	node=system.process_info.current_process;	
	current_process_context=node->val;
	
	if (flags & O_CREAT)
	{
		inode=alloc_inode(pathname,0,system->ext2);
		hashtable_put(current_process_context->file_desc,fd,inode);
	}
	else if (flags & O_APPEND)
	{
		fd=
	}
}

int close(int fd)
{

}

//return byte read
int read(int fd, void *buf, size_t count); 
{

}

//return write byte
 int write(int fd, const void *buf, size_t count)
{

}

void rm()
{

}

void mkdir()
{

}
