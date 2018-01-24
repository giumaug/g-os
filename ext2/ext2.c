#include "ext2/ext2.h"
#include "ext2/ext2_utils_1.h"
#include "ext2/ext2_utils_2.h"

static void indirect_block_free(t_indirect_block* indirect_block);
static t_indirect_block* indirect_block_init();

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
	u32 ret_code;
	struct t_process_context* current_process_context;
	t_inode* inode;
	t_llist_node* node;
	char path[NAME_MAX];
	char filename[NAME_MAX];

	inode = inode_init();
	CURRENT_PROCESS_CONTEXT(current_process_context);
	fd = current_process_context->next_fd++;
	//current_process_context->file_desc=kmalloc(sizeof(t_hashtable));
	//hashtable_init(current_process_context->file_desc,10);

	if (flags & O_CREAT & O_RDWR)
	{
		alloc_inode(fullpath,0,system.root_fs,inode);
		hashtable_put(current_process_context->file_desc,fd,inode);
	}
	else if (flags & (O_APPEND | O_RDWR))
	{
		ret_code = lookup_inode(fullpath,ext2,inode);		
		if (ret_code == -1)
		{
			return -1;
		}
		hashtable_put(current_process_context->file_desc,fd,inode);
	}
	else 
	{
		return -1;
	}
	inode->file_offset = 0;
	return fd;
}

int _close(t_ext2* ext2,int fd)
{
	u32 ret=-1;
	struct t_process_context* current_process_context;
	t_inode* inode;

	CURRENT_PROCESS_CONTEXT(current_process_context)
	inode=hashtable_remove(current_process_context->file_desc,fd);
//	if (inode != NULL)
//	{
//		ret = 0;
//		if (inode->indirect_block_1 != NULL)
//		{
//			indirect_block_free(inode->indirect_block_1);
//		}
//		if (inode->indirect_block_2 != NULL)
//		{
//			indirect_block_free(inode->indirect_block_2);
//		}
//		kfree(inode);
//	}
	ret = inode_free(inode);
	//AT THE MOMENT READ ONLY	
	//write_inode(system.root_fs,inode);
	return ret;
}

//int _read_orig(t_ext2* ext2,int fd, void* buf,u32 count)
//{
//	struct t_process_context* current_process_context;
//	u32 i;
//	u32 block_offset;
//	u32 first_inode_block;
//	u32 first_data_offset;
//	u32 last_inode_block;
//	u32 inode_block;
//	u32 lba;
//	u32 indirect_lba;
//	u32 allocated_indirect_block;
//	u32 sector_count;
//	u32 byte_to_read;
//	u32 byte_read;
//	u32 byte_count;
//	t_inode* inode;
//	char* iob_indirect_block;
//	char* iob_data_block;
//
//	byte_read=0;
//	byte_to_read=count;
//	iob_indirect_block=kmalloc(BLOCK_SIZE);
//	iob_data_block=kmalloc(BLOCK_SIZE);
//
//	CURRENT_PROCESS_CONTEXT(current_process_context)
//	inode=hashtable_get(current_process_context->file_desc,fd);
//	if (inode==NULL)
//	{
//		return -1;
//	}
//	first_inode_block=inode->file_offset/BLOCK_SIZE;
//	first_data_offset=inode->file_offset%BLOCK_SIZE;
//	last_inode_block=(inode->file_offset+count)/BLOCK_SIZE;
//
//	allocated_indirect_block=0;
//	for (i=first_inode_block;i<=last_inode_block;i++)
//	{
//		if (i>11)
//		{
//			if (!allocated_indirect_block)
//			{
//				indirect_lba=FROM_BLOCK_TO_LBA(inode->i_block[12]);
//        			sector_count=BLOCK_SIZE/SECTOR_SIZE;
//				READ(sector_count,indirect_lba,iob_indirect_block);
//				allocated_indirect_block=1;
//			}
//			READ_DWORD(&iob_indirect_block[4*(i-12)],inode_block);
//			lba=FROM_BLOCK_TO_LBA(inode_block);
//		}
//		else
//		{
//			lba=FROM_BLOCK_TO_LBA(inode->i_block[i]);
//		}
//		if (byte_to_read>=BLOCK_SIZE)
//		{
//			byte_count=BLOCK_SIZE;
//			byte_to_read-=BLOCK_SIZE;	
//		}
//		else
//		{
//			byte_count=byte_to_read;
//			byte_to_read=0;
//		}
//        	sector_count=BLOCK_SIZE/SECTOR_SIZE;
//		READ(sector_count,lba,iob_data_block);
//		kmemcpy(buf,iob_data_block+first_data_offset,byte_count);
//		inode->file_offset+=byte_count;
//		buf+=byte_count;
//		byte_read+=byte_count;
//	}
//	kfree(iob_indirect_block);
//	kfree(iob_data_block);
//	return byte_read;
//}

static t_indirect_block* indirect_block_init()
{
	int i = 0;
	t_indirect_block* indirect_block = NULL;

	indirect_block = kmalloc(sizeof(t_indirect_block));
	indirect_block->block = kmalloc(BLOCK_SIZE);
	indirect_block->block_map = kmalloc(BLOCK_SIZE * sizeof(t_indirect_block));
	for (i = 0 ;i <  BLOCK_SIZE; i++)
	{
		indirect_block->block_map[i] = NULL;
	}
	return indirect_block;
}

static void indirect_block_free(t_indirect_block* indirect_block)
{
	int i = 0;

	for (i = 0 ;i <  BLOCK_SIZE; i++)
	{
		if (indirect_block->block_map[i] != NULL)
		{
			indirect_block_free(indirect_block->block_map[i]);
		}
	}
	kfree(indirect_block->block_map);
	kfree(indirect_block->block);
	kfree(indirect_block);
}

void _read_test(t_ext2* ext2)
{
	int i;
	int lba;
	int sector_count;
	char* iob_data_block;
	char* buf;
	u32 cons_block;
	u32 first_block;
	u32 last_block;
	

	first_block = 3000;
	last_block = 33720;
	cons_block = 1;
	buf = kmalloc(1024 * cons_block);
	iob_data_block = kmalloc(1024 * cons_block);	

	for (i = first_block;i <= last_block;i += cons_block)
	{
		lba = i * (BLOCK_SIZE / SECTOR_SIZE);
		//printk("lba is: %d \n",lba);
		sector_count=(BLOCK_SIZE * cons_block) / SECTOR_SIZE;
		READ(sector_count,lba,iob_data_block);
		kmemcpy(buf,iob_data_block,(1024 * cons_block));
	}
	return;	
}

t_indirect_block* clone_indirect_block(t_indirect_block* indirect_block)
{
	int i;
	t_indirect_block* cloned_indirect_block = NULL;
	
	printk("using indirect \n");
	cloned_indirect_block = kmalloc(sizeof(t_indirect_block));
	cloned_indirect_block->block = kmalloc(BLOCK_SIZE);

	for (i = 0 ;i <  BLOCK_SIZE; i++)
	{
		if (indirect_block->block_map[i] != NULL)
		{
			 cloned_indirect_block->block_map[i] = clone_indirect_block(indirect_block->block_map[i]);
		}
		else
		{
			cloned_indirect_block->block_map[i] = NULL;
		}
	}
	for (i = 0 ;i <  BLOCK_SIZE; i++)
	{
		cloned_indirect_block->block = indirect_block->block;
	}
	return cloned_indirect_block;
}

t_hashtable* clone_file_desc(t_hashtable* file_desc)
{
	int i;
	t_inode* inode = NULL;
	t_hashtable* cloned_file_desc = NULL;

	printk("!!!! \n");
	cloned_file_desc = dc_hashtable_init(PROCESS_INIT_FILE,&inode_free);
	for (i = 0; i < file_desc->size;i++)
	{
		inode = hashtable_get(file_desc,i);
		if (inode != NULL)
		{
			hashtable_put(cloned_file_desc,i,inode_clone(inode)); 
		}
	}
	return cloned_file_desc;
}

t_inode* inode_clone(t_inode* inode)
{
	t_inode* cloned_inode = NULL;
	
	cloned_inode = inode_init();
	kmemcpy(cloned_inode,inode,sizeof(t_inode));
	if (inode->indirect_block_1 != NULL)
	{
		cloned_inode->indirect_block_1 = clone_indirect_block(inode->indirect_block_1);
	}
	if (inode->indirect_block_2 != NULL)
	{
		cloned_inode->indirect_block_2 = clone_indirect_block(inode->indirect_block_2);
	}
	return cloned_inode;
}

t_inode* inode_init()
{
	t_inode* inode = NULL;

	inode = kmalloc(sizeof(t_inode));
	inode->indirect_block_1 = NULL;
	inode->indirect_block_2 = NULL;
	return inode;
}

int inode_free(t_inode* inode)
{
	int ret = -1;
	
	if (inode != NULL)
	{
		if (inode->indirect_block_1 != NULL)
		{
			indirect_block_free(inode->indirect_block_1);
		}
		if (inode->indirect_block_2 != NULL)
		{
			indirect_block_free(inode->indirect_block_2);
		}
		ret = 0;
	}
	kfree(inode);
	return ret;
}

//int _read(t_ext2* ext2,int fd, void* buf,u32 count)
//{
//	struct t_process_context* current_process_context;
//	u32 i;
//	u32 block_offset;
//	u32 first_inode_block;
//	u32 first_data_offset;
//	u32 last_inode_block;
//	u32 inode_block;
//	u32 lba;
//	u32 indirect_lba;
//	u32 sector_count;
//	u32 byte_to_read;
//	u32 byte_read;
//	u32 byte_count;
//	t_inode* inode;
//	u32 inode_block_data; 
//	char* iob_data_block;
//	u32 second_block;
//	u32 second_block_offset;
//
//	byte_read=0;
//	byte_to_read=count;
////	iob_indirect_block=kmalloc(BLOCK_SIZE);
//	iob_data_block=kmalloc(BLOCK_SIZE);
//
//	CURRENT_PROCESS_CONTEXT(current_process_context)
//	inode=hashtable_get(current_process_context->file_desc,fd);
//	if (inode==NULL)
//	{
//		return -1;
//	}
//	first_inode_block=inode->file_offset/BLOCK_SIZE;
//	first_data_offset=inode->file_offset%BLOCK_SIZE;
//	last_inode_block=(inode->file_offset+(count-1))/BLOCK_SIZE;
//	
//	for (i=first_inode_block;i<=last_inode_block;i++)
//	{
//		if (i > INDIRECT_0_LIMIT && i <= INDIRECT_1_LIMIT)
//		{
//			if (inode->indirect_block_1 == NULL)
//			{
//				inode->indirect_block_1 = indirect_block_init();
//				indirect_lba = FROM_BLOCK_TO_LBA(inode->i_block[12]);
//        			sector_count = BLOCK_SIZE/SECTOR_SIZE;
//				READ(sector_count,indirect_lba,inode->indirect_block_1->block);
//			}
//			READ_DWORD(&inode->indirect_block_1->block[4*(i - INDIRECT_0_LIMIT - 1)],inode_block_data);
//			lba = FROM_BLOCK_TO_LBA(inode_block_data);
//		}
//		else if (i > INDIRECT_1_LIMIT  && i <= INDIRECT_2_LIMIT)
//		{
//			second_block = (i - INDIRECT_2_LIMIT - 1) / (BLOCK_SIZE / 4);
//			second_block_offset = (i - INDIRECT_2_LIMIT - 1) % (BLOCK_SIZE /4);
//			if (inode->indirect_block_2 == NULL)
//			{
//				inode->indirect_block_2 = indirect_block_init();
//				indirect_lba = FROM_BLOCK_TO_LBA(inode->i_block[13]);
//        			sector_count = BLOCK_SIZE/SECTOR_SIZE;
//				READ(sector_count,indirect_lba,inode->indirect_block_2->block);
//			}
//			if (inode->indirect_block_2->block_map[second_block] == NULL)
//			{
//				inode->indirect_block_2->block_map[second_block] = indirect_block_init();
//				indirect_lba = FROM_BLOCK_TO_LBA(inode->indirect_block_2->block[second_block]);
//        			sector_count = BLOCK_SIZE/SECTOR_SIZE;
//				READ(sector_count,indirect_lba,inode->indirect_block_2->block_map[second_block]->block);	
//			}
//			READ_DWORD(&inode->indirect_block_2->block_map[second_block]->block[second_block_offset],inode_block_data);
//			lba = FROM_BLOCK_TO_LBA(inode_block_data);
//		}
//		else if ( i > INDIRECT_2_LIMIT  && i <= INDIRECT_3_LIMIT )
//		{
//			//TO DO
//		}
//		else
//		{
//			lba=FROM_BLOCK_TO_LBA(inode->i_block[i]);
//		}
//		if (byte_to_read>=BLOCK_SIZE)
//		{
//			if (first_data_offset == 0)
//			{
//				byte_count=BLOCK_SIZE;
//				byte_to_read-=BLOCK_SIZE;
//			}
//			else
//			{
//				byte_count = BLOCK_SIZE - first_data_offset;
//				byte_to_read -= (BLOCK_SIZE - first_data_offset);
//			}	
//		}
//		else
//		{
//			if (first_data_offset == 0)
//			{
//				byte_count=byte_to_read;
//				byte_to_read=0;
//			}
//			else 
//			{
//				if (BLOCK_SIZE - first_data_offset > byte_to_read)
//				{
//					byte_count=byte_to_read;
//					byte_to_read=0;
//				}
//				else
//				{
//					byte_count = BLOCK_SIZE - first_data_offset;
//					byte_to_read -= (BLOCK_SIZE - first_data_offset);
//				}
//			}
//		}
//        	sector_count=BLOCK_SIZE/SECTOR_SIZE;
//		READ(sector_count,lba,iob_data_block);
//		kmemcpy(buf,iob_data_block+first_data_offset,byte_count);
//		inode->file_offset+=byte_count;
//		buf+=byte_count;
//		byte_read+=byte_count;
//		first_data_offset = 0;
//	}
//	//kfree(iob_indirect_block);
//	kfree(iob_data_block);
//	return byte_read;
//}

int _read(t_ext2* ext2,int fd, void* buf,u32 count,u8 is_dma)
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
	u32 sector_count;
	u32 byte_to_read;
	u32 byte_read;
	u32 byte_count;
	t_inode* inode;
	u32 inode_block_data; 
	char* iob_data_block;
	u32 second_block;
	u32 second_block_offset;
	t_llist* dma_lba_list = NULL;
	u32 last_lba = 0;
	u32 first_lba = 0;
	t_llist_node* sentinel = NULL;
	t_llist_node* next = NULL;
	t_dma_lba* dma_lba = NULL;
	u32 dma_sector_count = 0;
	u32 buf_offset;
	u32 dma_lba_list_size = 0;

	byte_read = 0;
	byte_to_read = count;

	CURRENT_PROCESS_CONTEXT(current_process_context)
	inode = hashtable_get(current_process_context->file_desc,fd);
	if (inode == NULL)
	{
		return -1;
	}
	first_inode_block = inode->file_offset / BLOCK_SIZE;
	first_data_offset = inode->file_offset % BLOCK_SIZE;
	last_inode_block = (inode->file_offset + (count - 1)) / BLOCK_SIZE;
	
	for (i = first_inode_block;i <= last_inode_block;i++)
	{
		if (i > INDIRECT_0_LIMIT && i <= INDIRECT_1_LIMIT)
		{
			if (inode->indirect_block_1 == NULL)
			{
				inode->indirect_block_1 = indirect_block_init();
				indirect_lba = FROM_BLOCK_TO_LBA(inode->i_block[12]);
        			sector_count = BLOCK_SIZE/SECTOR_SIZE;
				READ(sector_count,indirect_lba,inode->indirect_block_1->block);
			}
			READ_DWORD(&inode->indirect_block_1->block[4*(i - INDIRECT_0_LIMIT - 1)],inode_block_data);
			lba = FROM_BLOCK_TO_LBA(inode_block_data);
		}
		else if (i > INDIRECT_1_LIMIT  && i <= INDIRECT_2_LIMIT)
		{
			second_block = (i - INDIRECT_2_LIMIT - 1) / (BLOCK_SIZE / 4);
			second_block_offset = (i - INDIRECT_2_LIMIT - 1) % (BLOCK_SIZE /4);
			if (inode->indirect_block_2 == NULL)
			{
				inode->indirect_block_2 = indirect_block_init();
				indirect_lba = FROM_BLOCK_TO_LBA(inode->i_block[13]);
        			sector_count = BLOCK_SIZE/SECTOR_SIZE;
				READ(sector_count,indirect_lba,inode->indirect_block_2->block);
			}
			if (inode->indirect_block_2->block_map[second_block] == NULL)
			{
				inode->indirect_block_2->block_map[second_block] = indirect_block_init();
				indirect_lba = FROM_BLOCK_TO_LBA(inode->indirect_block_2->block[second_block]);
        			sector_count = BLOCK_SIZE/SECTOR_SIZE;
				READ(sector_count,indirect_lba,inode->indirect_block_2->block_map[second_block]->block);	
			}
			READ_DWORD(&inode->indirect_block_2->block_map[second_block]->block[second_block_offset],inode_block_data);
			lba = FROM_BLOCK_TO_LBA(inode_block_data);
		}
		else if ( i > INDIRECT_2_LIMIT  && i <= INDIRECT_3_LIMIT )
		{
			//TO DO
		}
		else
		{
			lba = FROM_BLOCK_TO_LBA(inode->i_block[i]);
		}
		if (byte_to_read >= BLOCK_SIZE)
		{
			if (first_data_offset == 0)
			{
				byte_count = BLOCK_SIZE;
				byte_to_read -= BLOCK_SIZE;
			}
			else
			{
				byte_count = BLOCK_SIZE - first_data_offset;
				byte_to_read -= (BLOCK_SIZE - first_data_offset);
			}	
		}
		else
		{
			if (first_data_offset == 0)
			{
				byte_count = byte_to_read;
				byte_to_read = 0;
			}
			else 
			{
				if (BLOCK_SIZE - first_data_offset > byte_to_read)
				{
					byte_count = byte_to_read;
					byte_to_read = 0;
				}
				else
				{
					byte_count = BLOCK_SIZE - first_data_offset;
					byte_to_read -= (BLOCK_SIZE - first_data_offset);
				}
			}
		}
		if(is_dma)
		{
			printk("lba is %d \n",lba);
			if(first_lba == 0 && i < last_inode_block)
			{
				first_lba = lba;
				last_lba = lba;
				dma_lba_list = new_dllist();
				dma_sector_count = (BLOCK_SIZE/SECTOR_SIZE);
			}
			else if(first_lba == 0 && i == last_inode_block)
			{
				dma_lba_list = new_dllist();
				dma_sector_count = (BLOCK_SIZE/SECTOR_SIZE);
				dma_lba_list_size = 1;
				dma_lba = kmalloc(sizeof(t_dma_lba));
				dma_lba->io_buffer = kmalloc(dma_sector_count * SECTOR_SIZE * 2);
				dma_lba->lba = lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list,dma_lba);
			}
			else if(lba == (last_lba + (BLOCK_SIZE/SECTOR_SIZE)) && i < last_inode_block)
			{
				dma_sector_count += (BLOCK_SIZE/SECTOR_SIZE);
				last_lba = lba;
			}
			else if (i == last_inode_block && lba == (last_lba + (BLOCK_SIZE/SECTOR_SIZE)))
			{
				dma_sector_count += (BLOCK_SIZE/SECTOR_SIZE);
				dma_lba_list_size++;
				dma_lba = kmalloc(sizeof(t_dma_lba));
				dma_lba->io_buffer = kmalloc(dma_sector_count * SECTOR_SIZE * 2);
				dma_lba->lba = first_lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list,dma_lba);
			}
			else if (i == last_inode_block && lba != (last_lba + (BLOCK_SIZE/SECTOR_SIZE)))
			{
				dma_lba_list_size++;
				dma_lba = kmalloc(sizeof(t_dma_lba));
				dma_lba->io_buffer = kmalloc(dma_sector_count * SECTOR_SIZE * 2);
				dma_lba->lba = first_lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list,dma_lba);

				dma_sector_count = (BLOCK_SIZE/SECTOR_SIZE);
				dma_lba = kmalloc(sizeof(t_dma_lba));
				dma_lba->io_buffer = kmalloc(dma_sector_count * SECTOR_SIZE * 2);
				dma_lba->lba = lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list,dma_lba);
			}
			else
			{
				dma_lba_list_size++;
				dma_lba = kmalloc(sizeof(t_dma_lba));
				dma_lba->io_buffer = kmalloc(dma_sector_count * SECTOR_SIZE * 2);
				dma_lba->lba = first_lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list,dma_lba);
				dma_sector_count = 0;
				last_lba = lba;
				first_lba = lba;
			}
		}
		else
		{
			iob_data_block = kmalloc(BLOCK_SIZE);
        		sector_count = BLOCK_SIZE/SECTOR_SIZE;
			READ(sector_count,lba,iob_data_block);
			kmemcpy(buf,iob_data_block+first_data_offset,byte_count);
			inode->file_offset += byte_count;
			buf += byte_count;
			byte_read += byte_count;
			first_data_offset = 0;
		}
	}
	if (is_dma)
	{
		READ_DMA(dma_lba_list,dma_lba_list_size);
		buf_offset = 0;
		byte_count = 0;
		sentinel = ll_sentinel(dma_lba_list);
		next = ll_first(dma_lba_list);
		while(sentinel != next)
		{
			dma_lba = next->val;
			byte_count = dma_lba->sector_count * SECTOR_SIZE;
			kmemcpy(buf + buf_offset,dma_lba->io_buffer,byte_count);
			kfree(dma_lba->io_buffer);
			buf_offset += byte_count;
			next = ll_next(next);
		}
		inode->file_offset += byte_count;
		free_llist(dma_lba_list);
	}
	else
	{
		kfree(iob_data_block);
	}
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
	inode=hashtable_get(((struct t_process_context*)system.process_info->current_process)->file_desc,fd);
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
	if (whence==SEEK_SET)
	{
		if (offset>inode->i_size || offset<0)
		{
			return -1;
		}
		inode->file_offset=offset;
	}
	else if (whence==SEEK_CUR)
	{
		if (inode->file_offset+offset>inode->i_size || inode->file_offset+offset<0)
		{
			return -1;
		}
		inode->file_offset+=offset;
	}
	else if (whence==SEEK_END)
	{
		if (inode->i_size+offset>inode->i_size || inode->i_size+offset<0)
		{
			return -1;
		}
		inode->i_size+=offset;
	}

//	CURRENT_PROCESS_CONTEXT(current_process_context);	
//	inode=hashtable_get(current_process_context->file_desc,fd);
//	inode->file_offset=offset;
	return offset;
}

//HO TOLTO PARENT_DIR DA LOOKUP_INODE DA RIVEDERE  _RM
int _rm(t_ext2* ext2,char* fullpath)
{
	return 0;
}

//NON CANCELLARE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//int _rm(t_ext2* ext2,char* fullpath)
//{
//	t_inode* inode;
//	char path[NAME_MAX];
//	char filename[NAME_MAX];
//
//	inode=kmalloc(sizeof(t_inode));
//	
//	extract_filename(fullpath,path,filename);
//	lookup_inode(filename,ext2,inode);
//	del_dir_entry(ext2,inode_dir,inode->i_number);
//	free_inode(inode,ext2);
//
//	kfree(inode);
//	return 0;
//}


//HO TOLTO PARENT_DIR DA LOOKUP_INODE DA RIVEDERE  _MKDIR
int _mkdir(t_ext2* ext2,const char* fullpath)
{

}
//NON CANCELLARE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//int _mkdir(t_ext2* ext2,const char* fullpath)
//{
//	t_inode* inode;
//	t_inode* inode_parent_dir;
//	char* iob_dir;
//	char path[NAME_MAX];
//	char filename[NAME_MAX];
//	u32 sector_count;
//	
//	inode=kmalloc(sizeof(t_inode));
//	inode_parent_dir=kmalloc(sizeof(t_inode));
//	iob_dir=kmalloc(BLOCK_SIZE);
//	kfillmem(iob_dir,0,BLOCK_SIZE);
//		
//	alloc_inode(fullpath,1,ext2,inode);
//	lookup_inode(fullpath,ext2,inode_parent_dir,inode);
//	inode->i_block[0]=alloc_block(ext2,inode,0);
//	
//	iob_dir[0]=inode->i_number;
//	iob_dir[4]=12;
//	iob_dir[6]=1;
//	iob_dir[7]=2;
//	iob_dir[8]='.';
//	iob_dir[9]='\0';
//	iob_dir[10]='\0';
//	iob_dir[11]='\0';
//	
//	iob_dir[12]=inode_parent_dir->i_number;
//	iob_dir[16]=24;
//	iob_dir[18]=1;
//	iob_dir[19]=2;
//	iob_dir[20]='.';
//	iob_dir[21]='.';
//	iob_dir[22]='\0';
//	iob_dir[23]='\0';
//
//	extract_filename(fullpath,path,filename);
//	sector_count=BLOCK_SIZE/SECTOR_SIZE;
//	WRITE(sector_count,inode->i_block[0],iob_dir);
//	add_dir_entry(ext2,inode_parent_dir,inode->i_number,filename,2);
//
//	kfree(inode);
//	kfree(inode_parent_dir);
//	kfree(iob_dir);
//	return 0;
//}

int _chdir(t_ext2* ext2,char* path)
{
	u32 current_dir_inode;
	u32 ret;
	t_inode* inode;
	struct t_process_context* current_process_context;

	inode = inode_init();
	CURRENT_PROCESS_CONTEXT(current_process_context)
	ret=lookup_inode(path,ext2,inode);
	if (ret==0)
	{
		current_process_context->current_dir_inode_number=inode->i_number;
	}
	//kfree(inode);
	inode_free(inode);
}

int _stat(t_ext2* ext2,char* pathname,t_stat* stat)
{
	t_inode* inode;
	int ret_code=-1;

	inode = inode_init();
	ret_code=lookup_inode(pathname,ext2,inode);		
	if (ret_code==-1)
	{
		//kfree(inode);
		inode_free(inode);
		return -1;
	}
       
	stat->st_ino=inode->i_number;
	stat->st_mode=inode->i_mode;
	stat->st_uid=inode->i_uid;
	stat->st_gid=inode->i_gid;
	stat->st_size=inode->i_size;
	stat->st_atime=inode->i_atime;
	stat->st_mtime=inode->i_mtime;
	stat->st_ctime=inode->i_ctime;

	//kfree(inode);
	inode_free(inode);
	return 0;	
}
