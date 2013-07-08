#include "data_types/hashtable.h"
#include "ext2/ext2.h"
#include "ext2/ext2_utils_1.h"
#include "ext2/ext2_utils_2.h"

int open(const char *path, int flags, mode_t mode); 
{
	//se file esiste carico inode da disco 
	//se file non esiste alloco inode
	u32 fd;
	struct t_process_context* current_process_context;
	t_inode* inode;
	t_llist_node* node;

	inode=kmalloc(sizeof(t_inode);
	node=system.process_info.current_process;	
	current_process_context=node->val;
	fd=current_process_context->next_fd++;
	
	if (flags & O_CREAT)
	{
		inode=alloc_inode(path,0,system->ext2,inode);
		hashtable_put(current_process_context->file_desc,fd,inode);
	}
	else if (flags & O_APPEND)
	{
		lookup_inode(path,ext2,inode);
		hashtable_put(current_process_context->file_desc,fd,inode);
	}
	inode->file_offset=0;
	return fd;
}

int close(int fd)
{
	t_inode* inode;
	
	inode=hashtable_get(current_process_context->file_desc,fd);

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
