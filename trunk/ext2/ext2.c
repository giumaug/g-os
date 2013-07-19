#include "data_types/hashtable.h"
#include "ext2/ext2.h"
#include "ext2/ext2_utils_1.h"
#include "ext2/ext2_utils_2.h"

//mettere inode dentro dir
int open(const char *path, int flags, mode_t mode); 
{
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
	write_inode(system->ext2,inode);
	kfree(inode);
}

int read(int fd, void *buf, size_t count); 
{
	u32 i;	
	u32 first_inode_block;
	u32 first_data_offset;
	u32 last_inode_block;
	u32 inode_block;
	u32 lba;
	u32 indirect_lba;
	u32 allocated_indirect_block;
	u32 sector_count;
	u32 byte_to_read;
	u32 byte_read;
	u32 byte_count;
	t_inode* inode;
	void* iob_indirect_block;
	void* iob_data_block;

	byte_read=0;
	iob_indirect_block=kmalloc(BLOCK_SIZE);
	iob_data_block=kmalloc(BLOCK_SIZE);
	inode=hashtable_get(current_process_context->file_desc,fd);
	first_inode_block=inode->file_offset/BLOCK_SIZE;
	first_data_offset=inode->file_offset%BLOCK_SIZE;
	last_inode_block=(inode->file_offset+count)/BLOCK_SIZE;

	allocated_indirect_block=0;
	for (i=first_inode_block;i<=first_inode_block;i++)
	{
		if (i>12)
		{
			if (!allocated_indirect_block)
			{
				indirect_lba=inode->i_block[12];
        			sector_count=BLOCK_SIZE/SECTOR_SIZE;
				_read_28_ata(sector_count,indirect_lba,iob_indirect_block,TRUE);
				allocated_indirect_block=1;
			}
			lba=iob_indirect_block[inode_block-12];	
		}
		else
		{
			lba=i;
		}
		if (byte_to_read>=(BLOCK_SIZE/SECTOR_SIZE))
		{
			byte_count=BLOCK_SIZE/SECTOR_SIZE;
			byte_to_read-=BLOCK_SIZE/SECTOR_SIZE;	
		}
		else
		{
			byte_count=byte_to_read;
		}
        	sector_count=BLOCK_SIZE/SECTOR_SIZE;
		_read_28_ata(sector_count,lba,iob_data_block,TRUE);
		if(i==first_inode_block)
		{
			buf+=first_inode_block;
		}
		kmemcpy(buf,iob_data_block,byte_count);
		inode->file_offset+=byte_to_read;
		buf+=byte_to_read;
		byte_read+=byte_count;
	}
	kfree(iob_indirect_block);
	kfree(iob_data_block);
	return byte_read;
}

 int write(int fd, const void *buf, size_t count)
{
	{
	u32 i;	
	u32 first_inode_block;
	u32 first_data_offset;
	u32 last_inode_block;
	u32 last_data_offset;
	u32 inode_block;
	u32 lba;
	u32 indirect_lba;
	u32 indirect_old_lba;
	u32 sector_count;
	u32 byte_to_read;
	u32 byte_read;
	u32 byte_count;
	u32 load_block;
	t_inode* inode;
	void* iob_data_block;
	void* iob_indirect_block;

	iob_data_block=kmalloc(BLOCK_SIZE);
	iob_indirect_block=kmalloc(BLOCK_SIZE);
	kfillmem(iob_data_block,0,BLOCK_SIZE);
	kfillmem(iob_indirect_block,0,BLOCK_SIZE);

	byte_written=0;
	inode=hashtable_get(current_process_context->file_desc,fd);
	first_inode_block=inode->file_offset/BLOCK_SIZE;
	first_data_offset=inode->file_offset%BLOCK_SIZE;
	last_inode_block=(inode->file_offset+count)/BLOCK_SIZE;
	last_data_offset=(inode->file_offset+count)%BLOCK_SIZE;

	byte_to_write=count;

	if (last_inode_block>=12)
	{
		if (inode->i_block[12]==0)
		{
			inode->i_block[12]=alloc_indirect_block(ext2,inode);
		}
		else
		{
			sector_count=BLOCK_SIZE/SECTOR_SIZE;
			_read_28_ata(sector_count,inode->i_block[12],iob_indirect_block,TRUE);
		}
	}

	for (i=first_inode_block;i<=last_inode_block;i++)
	{
		if ((i==first_inode_block && first_data_offset!=0) || (i==last_inode_block && last_data_offset!=0))
		{
			load_block=1;
			if (i>12)
			{
				if (iob_indirect_block[i-12]==0)
				{
					iob_indirect_block[i-12]=alloc_block(ext2,inode,i);
					load_block=0;
				}
				lba=iob_indirect_block[i-12];
			}
			else
			{
				if (inode->i_block[i]==0)
				{
					inode->i_block[i]=alloc_block(ext2,inode,i);
					load_block=0;
				}
				lba=inode->i_block[i];
			}
			if (load_block)
			{
				sector_count=BLOCK_SIZE/SECTOR_SIZE;
				_read_28_ata(sector_count,lba,iob_data_block,TRUE);
				if (i==first_inode_block)
				{
					iob_data_block+=first_data_offset;
					byte_count=BLOCK_SIZE-first_data_offset;
					byte_to_write-=byte_count;
				}
				else if (i==last_inode_block)
				{
					byte_count=last_data_offset;
					byte_to_write-=byte_count;
				}	
			}
		}
		else
		{
			byte_count=BLOCK_SIZE;
			byte_to_write-=BLOCK_SIZE;		
			if (i>12)
			{
				if (iob_indirect_block[i-12]==0)
				{
					iob_indirect_block[i-12]=alloc_block(ext2,inode,i);
				}
				lba=iob_indirect_block[i-12];
			}
			else
			{
				if (inode->i_block[i]==0)
				{
					inode->i_block[i]=alloc_block(ext2,inode,i);
				}
				lba=inode->i_block[i];
			}
			byte_count=BLOCK_SIZE;
			byte_to_write-=BLOCK_SIZE;
		}
		kmemcpy(iob_data_block,buf,byte_count);
		sector_count=BLOCK_SIZE/SECTOR_SIZE;
		_write_28_ata(sector_count,lba,io_buffer_2,TRUE);
		inode->file_offset+=byte_count;
		buf+=byte_count;
		byte_written+=byte_count;
	}
	kfree(iob_data_block);
	kfree(iob_indirect_block);
	return byte_written;
}

---invalidare dir
int rm(t_ext2* ext2,char* path)
{
	t_inode* inode;

	inode=kmalloc(sizeof(t_inode));
	lookup_inode(path,ext2,inode);
	free_inode(inode,ext2);
	kfree(inode);
	return 0;
}

---qui
int mkdir()
{
	t_inode* inode;
	t_inode* inode_parent_dir;
	void* iob_dir;
	
	inode=kmalloc(sizeof(t_inode));
	inode_parent_dir=kmalloc(sizeof(t_inode));
	iob_dir=kmalloc(BLOCK_SIZE);
	kfillmem(iob_dir,0,BLOCK_SIZE);
		
	alloc_inode(path,1,ext2,inode);
	lookup_inode(cpath,ext2,inode_parent_dir);
	inode->i_block[0]=inode->inode_number;
	
	iob_dir[0]=inode->inode_number;
	iob_dir[4]=inode_parent_dir->inode_number;
	iob_dir[6]=
	iob_dir[7]=
	iob_dir[8]=


	kfree(inode);
	kfree(inode_parent_dir);
}
