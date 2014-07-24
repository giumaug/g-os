#include "ext2/ext2.h"
#include "ext2/ext2_utils_1.h"
#include "ext2/ext2_utils_2.h"

void init_ext2(t_ext2 *ext2,t_device_desc* device_desc)
{	
	t_group_block* group_block;
	ext2->device_desc=device_desc;
	ext2->superblock=kmalloc(sizeof(t_superblock));
        ext2->partition_start_sector=lookup_partition(ext2,1);        
        read_superblock(ext2);
	//ext2->root_dir_inode=kmalloc(sizeof(t_inode));
	//read_root_dir_inode(ext2);
	ext2->device_desc=device_desc;
}

void free_ext2(t_ext2* ext2)
{
	kfree(ext2->superblock);
	//kfree(ext2->root_dir_inode);
}

int _open(t_ext2* ext2,const char* fullpath, int flags)
{
	u32 fd;
	struct t_process_context* current_process_context;
	t_inode* inode;
	t_inode* inode_dir;
	t_llist_node* node;
	char path[NAME_MAX];
	char filename[NAME_MAX];

	inode=kmalloc(sizeof(t_inode));
	inode_dir=kmalloc(sizeof(t_inode));
//	node=system.process_info.current_process;	
//	current_process_context=node->val;
	CURRENT_PROCESS_CONTEXT(current_process_context);
	fd=current_process_context->next_fd++;
	current_process_context->file_desc=kmalloc(sizeof(t_hashtable));
	hashtable_init(current_process_context->file_desc,10);

	if (flags & O_CREAT & O_RDWR)
	{
		alloc_inode(fullpath,0,system.root_fs,inode);
		hashtable_put(current_process_context->file_desc,fd,inode);
	}
	else if (flags & (O_APPEND | O_RDWR))
	{
		lookup_inode(fullpath,ext2,inode_dir,inode);		
		//add_dir_entry(ext2,inode_dir,inode->i_number,filename,1); ????
		hashtable_put(current_process_context->file_desc,fd,inode);
	}
	inode->file_offset=0;
	return fd;
}

int _close(t_ext2* ext2,int fd)
{
	struct t_process_context* current_process_context;
	t_inode* inode;

	CURRENT_PROCESS_CONTEXT(current_process_context)
	inode=hashtable_get(current_process_context->file_desc,fd);
	//AT THE MOMENT READ ONLY	
	//write_inode(system.root_fs,inode);
	kfree(inode);
}

int _read(t_ext2* ext2,int fd, void* buf,u32 count)
{
	struct t_process_context* current_process_context;
	u32 i;
	u32 block_offset;
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
	char* iob_indirect_block;
	char* iob_data_block;

	byte_read=0;
	byte_to_read=count;
	iob_indirect_block=kmalloc(BLOCK_SIZE);
	iob_data_block=kmalloc(BLOCK_SIZE);

	CURRENT_PROCESS_CONTEXT(current_process_context)
	inode=hashtable_get(current_process_context->file_desc,fd);
	first_inode_block=inode->file_offset/BLOCK_SIZE;
	first_data_offset=inode->file_offset%BLOCK_SIZE;
	last_inode_block=(inode->file_offset+count)/BLOCK_SIZE;

	allocated_indirect_block=0;
	for (i=first_inode_block;i<=last_inode_block;i++)
	{
		if (i>11)
		{
			if (!allocated_indirect_block)
			{
				indirect_lba=FROM_BLOCK_TO_LBA(inode->i_block[12]);
        			sector_count=BLOCK_SIZE/SECTOR_SIZE;
				READ(sector_count,indirect_lba,iob_indirect_block);
				allocated_indirect_block=1;
			}
			READ_DWORD(&iob_indirect_block[4*(i-12)],inode_block);
			lba=FROM_BLOCK_TO_LBA(inode_block);
		}
		else
		{
			lba=FROM_BLOCK_TO_LBA(inode->i_block[i]);
		}
		if (byte_to_read>=BLOCK_SIZE)
		{
			byte_count=BLOCK_SIZE;
			byte_to_read-=BLOCK_SIZE;	
		}
		else
		{
			byte_count=byte_to_read;
			byte_to_read=0;
		}
        	sector_count=BLOCK_SIZE/SECTOR_SIZE;
		READ(sector_count,lba,iob_data_block);
		kmemcpy(buf,iob_data_block+first_data_offset,byte_count);
		inode->file_offset+=byte_count;
		buf+=byte_count;
		byte_read+=byte_count;
	}
	kfree(iob_indirect_block);
	kfree(iob_data_block);
	return byte_read;
}

int _write(t_ext2* ext2,int fd, const void *buf,u32 count)
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
	u32 update_indirct_block;
	t_inode* inode;
	char* iob_data_block;
	char* iob_indirect_block;
	u32 update_indirect_block;
	u32 byte_written;
	u32 byte_to_write;

	iob_data_block=kmalloc(BLOCK_SIZE);
	iob_indirect_block=kmalloc(BLOCK_SIZE);
	kfillmem(iob_data_block,0,BLOCK_SIZE);
	kfillmem(iob_indirect_block,0,BLOCK_SIZE);

	update_indirect_block=FALSE;
	byte_written=0;
	inode=hashtable_get(((struct t_process_context*)system.process_info.current_process)->file_desc,fd);
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
			READ(sector_count,inode->i_block[12],iob_indirect_block);
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
					update_indirect_block=TRUE;
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
				_read_28_ata(sector_count,lba,iob_data_block);
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
					update_indirect_block=TRUE;
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
		WRITE(sector_count,lba,iob_data_block);
		inode->file_offset+=byte_count;
		buf+=byte_count;
		byte_written+=byte_count;
	}
	if (update_indirect_block)
	{
		WRITE(sector_count,inode->i_block[12],iob_indirect_block);
	}
	kfree(iob_data_block);
	kfree(iob_indirect_block);
	return byte_written;
}

int _seek(t_ext2* ext2,int fd,unsigned int offset,int whence)
{
	struct t_process_context* current_process_context;
	t_inode* inode;

	CURRENT_PROCESS_CONTEXT(current_process_context);	
	inode=hashtable_get(current_process_context->file_desc,fd);
	// ONLY SEEK_SET AT THE MOMENT
	inode->file_offset=offset;
	return offset;
}

int _rm(t_ext2* ext2,char* fullpath)
{
	t_inode* inode;
	t_inode* inode_dir;
	char path[NAME_MAX];
	char filename[NAME_MAX];

	inode=kmalloc(sizeof(t_inode));
	inode_dir=kmalloc(sizeof(t_inode));
	
	extract_filename(fullpath,path,filename);
	//lookup_inode(path,ext2,NULL,inode_dir);
	lookup_inode(filename,ext2,inode_dir,inode);
	del_dir_entry(ext2,inode_dir,inode->i_number);
	free_inode(inode,ext2);

	kfree(inode);
	kfree(inode_dir);
	return 0;
}

int _mkdir(t_ext2* ext2,const char* fullpath)
{
	t_inode* inode;
	t_inode* inode_parent_dir;
	char* iob_dir;
	char path[NAME_MAX];
	char filename[NAME_MAX];
	u32 sector_count;
	
	inode=kmalloc(sizeof(t_inode));
	inode_parent_dir=kmalloc(sizeof(t_inode));
	iob_dir=kmalloc(BLOCK_SIZE);
	kfillmem(iob_dir,0,BLOCK_SIZE);
		
	alloc_inode(fullpath,1,ext2,inode);
	lookup_inode(fullpath,ext2,inode_parent_dir,inode);
	inode->i_block[0]=alloc_block(ext2,inode,0);
	
	iob_dir[0]=inode->i_number;
	iob_dir[4]=12;
	iob_dir[6]=1;
	iob_dir[7]=2;
	iob_dir[8]='.';
	iob_dir[9]='\0';
	iob_dir[10]='\0';
	iob_dir[11]='\0';
	
	iob_dir[12]=inode_parent_dir->i_number;
	iob_dir[16]=24;
	iob_dir[18]=1;
	iob_dir[19]=2;
	iob_dir[20]='.';
	iob_dir[21]='.';
	iob_dir[22]='\0';
	iob_dir[23]='\0';

	extract_filename(fullpath,path,filename);
	sector_count=BLOCK_SIZE/SECTOR_SIZE;
	WRITE(sector_count,inode->i_block[0],iob_dir);
	add_dir_entry(ext2,inode_parent_dir,inode->i_number,filename,2);

	kfree(inode);
	kfree(inode_parent_dir);
	kfree(iob_dir);
	return 0;
}

//manca cd
