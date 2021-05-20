#include "lib/lib.h"
#include "common.h"
#include "drivers/ata/ata.h"
#include "ext2/ext2.h"
#include "ext2/ext2_utils_1.h"
#include "ext2/ext2_utils_2.h"
#include "ext2/ext2_inode_cache.h"

static void indirect_block_free(t_indirect_block* indirect_block);
static t_indirect_block* indirect_block_init();
static void inode_decrease_counter(t_inode* inode);


void init_ext2(t_ext2 *ext2,t_device_desc* device_desc)
{
	int i;
	u32 group_block_num;
	t_group_block* group_block = NULL;
	struct t_sem_desc* sem_desc = NULL;

	sem_desc = kmalloc(sizeof(t_sem_desc));
	ext2->sem = sem_desc;
	ext2->device_desc = device_desc;
	ext2->superblock = kmalloc(sizeof(t_superblock));
    ext2->partition_start_sector = lookup_partition(ext2,1);  
    read_superblock(ext2);

	group_block_num = ext2->superblock->s_blocks_count / (BLOCK_SIZE * 8);
	ext2->superblock->group_block_num = group_block_num;
	ext2->superblock->group_block_list = kmalloc(sizeof(char*) * group_block_num);
	ext2->superblock->group_block_list_status = kmalloc(sizeof(char) * group_block_num);
	ext2->superblock->group_block_bitmap_list = kmalloc(sizeof(char*) * group_block_num);
	ext2->superblock->group_block_bitmap_list_status = kmalloc(sizeof(char) * group_block_num);
	ext2->superblock->group_inode_bitmap_list = kmalloc(sizeof(char*) * group_block_num);
	ext2->superblock->group_inode_bitmap_list_status = kmalloc(sizeof(char) * group_block_num);

	for (i = 0; i < group_block_num; i++)
	{
		ext2->superblock->group_block_list[i] = NULL;
		ext2->superblock->group_block_list_status[i] = 0;
		ext2->superblock->group_block_bitmap_list[i] = NULL;
		ext2->superblock->group_block_bitmap_list_status[i] = 0;
		ext2->superblock->group_inode_bitmap_list[i] = NULL;
		ext2->superblock->group_inode_bitmap_list_status[i] = 0;
	}
	ext2->superblock->inode_cache = init_inode_cache();
	ext2->device_desc = device_desc;
	sem_init(ext2->sem, 1);
}

void free_ext2(t_ext2* ext2)
{
	u32 i;

	for (i = 0; i < ext2->superblock->group_block_num; i++)
	{
		if (ext2->superblock->group_block_list[i] != NULL)
		{
			kfree(ext2->superblock->group_block_list[i]);
		}
		if (ext2->superblock->group_block_bitmap_list[i] != NULL)
		{
			kfree(ext2->superblock->group_block_bitmap_list[i]);
		}
		if (ext2->superblock->group_inode_bitmap_list[i] != NULL)
		{
			kfree(ext2->superblock->group_inode_bitmap_list[i]);
		}
	}
	kfree(ext2->superblock->group_block_list);
	kfree(ext2->superblock->group_block_list_status);
	kfree(ext2->superblock->group_block_bitmap_list);
	kfree(ext2->superblock->group_block_bitmap_list_status);
	kfree(ext2->superblock->group_inode_bitmap_list);
	kfree(ext2->superblock->group_inode_bitmap_list_status);
	free_inode_cache(ext2->superblock->inode_cache); 
	kfree(ext2->superblock);
	kfree(ext2->sem);
}

int _open(t_ext2* ext2, const char* full_path, int flags)
{
	int ret;
	u32 fd;
	//u32 ret_code;
	struct t_process_context* current_process_context = NULL;
	t_inode* inode = NULL;
	t_inode* inode_parent_dir = NULL;
	t_llist_node* node = NULL;
	char parent_path[NAME_MAX];
	char file_name[NAME_MAX];

	//inode = inode_init(ext2);
	CURRENT_PROCESS_CONTEXT(current_process_context);
	fd = current_process_context->next_fd++;
	inode = lookup_inode(full_path,ext2);
	
	if (inode == NULL)
	{
		if (flags & O_CREAT)
		{
			inode = inode_init(ext2);
			//inode->parent_dir = inode_init(ext2);
			find_parent_path_and_filename(full_path, parent_path, file_name);
			inode_parent_dir = lookup_inode(parent_path, ext2);
			inode->i_number = alloc_inode(inode_parent_dir, 0, ext2);
			if (inode->i_number == -1)
			{
				inode_free(inode);
				panic();
				return -1;
			}
			ret = add_entry_to_dir(file_name, inode_parent_dir, ext2,inode->i_number, FILE);
			if (ret == -1)
			{
				inode_prealloc_block_free(inode, 1);
				inode_free(inode);
				panic();
				return -1;
			}
			hashtable_put(current_process_context->file_desc, fd, inode);
			inode->counter++;
//			if (inode->parent_dir != NULL)
//			{
//				inode->parent_dir->counter++;
//			}
		}
		else
		{
			return -1;
		}
		inode->i_mode = 0x81FF;
	    inode->i_links_count = 1;
        inode->i_atime = 0;
		inode->i_ctime = 0;
		inode->i_mtime = 0;
		inode->i_dtime = 0;
		write_inode(ext2, inode, 1, 1);
	}
	else
	{
		hashtable_put(current_process_context->file_desc, fd, inode);
		inode->file_offset = 0;
		inode->counter++;
//		if (inode->parent_dir != NULL)
//		{
//			inode->parent_dir->counter++;
//		}
	}
	return fd;
}

int _close(t_ext2* ext2, int fd)
{
	u32 ret = -1;
	struct t_process_context* current_process_context = NULL;
	t_inode* inode = NULL;

	CURRENT_PROCESS_CONTEXT(current_process_context)
	inode = hashtable_remove(current_process_context->file_desc, fd);
	//write_inode(ext2, inode, 1, 1);
	inode->counter--;
//	if (inode->counter == 0)
//	{
//		inode_prealloc_block_free(inode, 1);
//		inode_free(inode);
//	}
//	if (inode->parent_dir != NULL)
//	{
//		inode->parent_dir->counter--;
//		if (inode->parent_dir->counter == 0)
//		{
//			inode_prealloc_block_free(inode->parent_dir, 1);
//			inode_free(inode->parent_dir);
//		}
//	}
	sync_fs(ext2);
	return ret;
}

static void sync_fs(t_ext2* ext2)
{
	u8 i;
	u8 group_block_num;
	t_group_block* group_block = NULL;
	char* io_buffer = NULL;

	sem_down(ext2->sem);
	for (i = 0; i < ext2->superblock->group_block_num; i++ )
	{
		if (ext2->superblock->group_block_list_status[i] == 1)
		{
			group_block = ext2->superblock->group_block_list[i];
			write_group_block(ext2, i, group_block, 1);
		}
		if (ext2->superblock->group_block_bitmap_list_status[i] == 1)
		{
			if (ext2->superblock->group_block_list[i] == NULL)
			{
				group_block = read_group_block(ext2, i);
			}
			io_buffer = ext2->superblock->group_block_bitmap_list[i];
			write_block_bitmap(ext2, group_block->bg_block_bitmap, io_buffer, i, 1);
		}
		if (ext2->superblock->group_inode_bitmap_list_status[i] == 1)
		{
			if (ext2->superblock->group_block_list[i] == NULL)
			{
				group_block = read_group_block(ext2, i);
			}
			io_buffer = ext2->superblock->group_inode_bitmap_list[i];
			write_inode_bitmap(ext2, group_block->bg_inode_bitmap, io_buffer, i, 1);
		}
	}
	write_superblock(ext2);
	sem_up(ext2->sem);
}

static t_indirect_block* indirect_block_init()
{
	int i = 0;
	t_indirect_block* indirect_block = NULL;

	indirect_block = kmalloc(sizeof(t_indirect_block));
	indirect_block->block = kmalloc(BLOCK_SIZE);
	indirect_block->block_map = kmalloc((BLOCK_SIZE / 4) * sizeof(t_indirect_block*));
	for (i = 0 ;i <  (BLOCK_SIZE / 4); i++)
	{
		indirect_block->block_map[i] = NULL;
		indirect_block->block[i] = NULL;
	}
	return indirect_block;
}

static void indirect_block_free(t_indirect_block* indirect_block)
{
	int i = 0;

	for (i = 0 ;i <  (BLOCK_SIZE / 4); i++)
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

t_hashtable* clone_file_desc(t_hashtable* file_desc)
{
	int i;
	t_inode* inode = NULL;
	t_hashtable* cloned_file_desc = NULL;

	cloned_file_desc = dc_hashtable_init(PROCESS_INIT_FILE, &inode_decrease_counter);
	for (i = 0; i < file_desc->size;i++)
	{
		inode = hashtable_get(file_desc, i);
		if (inode != NULL)
		{
			inode->counter++;
			hashtable_put(cloned_file_desc, i, inode);
//			if (inode->parent_dir != NULL)
//			{
//				inode->parent_dir->counter++;
//			}
		}
	}
	return cloned_file_desc;
}

t_inode* inode_init(t_ext2* ext2)
{
	int i;
	t_inode* inode = NULL;

	inode = kmalloc(sizeof(t_inode));
	inode->counter = 0;
	inode->i_number = 0;
	inode->last_block_num = 0;
	inode->last_file_block_num = -1;
	inode->file_offset = 0;
	inode->preallocated_block_count = 0;
	inode->first_preallocated_block = -1;
	inode->indirect_block_1 = NULL;
	inode->indirect_block_2 = NULL;
	//inode->parent_dir = NULL;
	inode->ext2 = ext2;
  	inode->i_mode = 0;
	inode->i_uid = 0;
	inode->i_size = 0;
	inode->i_atime = 0;
	inode->i_ctime = 0;
	inode->i_mtime = 0;
	inode->i_dtime = 0;
	inode->i_gid = 0;
	inode->i_links_count = 0;
	inode->i_blocks = 0;
	inode->i_flags = 0;
	inode->osd1 = 0;
	for (i = 0; i < EXT2_N_BLOCKS; i++)
	{
		inode->i_block[i] = NULL;
	}
	inode->i_generation = 0;
	inode->i_file_acl = 0;
	inode->i_dir_acl = 0;
	inode-> i_faddr = 0;
	inode->osd2_1 = 0;
	inode->osd2_2 = 0;
	inode->osd2_3 = 0;
	return inode;
}

void inode_prealloc_block_free(t_inode* inode, u32 lock)
{
	char* io_buffer = NULL;
	t_group_block* group_block = NULL;
	u32 group_block_index;
	
	if (inode->preallocated_block_count > 0)
	{
		if (lock)
		{
			sem_down(inode->ext2->sem);
		}
		group_block_index = inode->first_preallocated_block / inode->ext2->superblock->s_blocks_per_group;
		group_block = read_group_block(inode->ext2, group_block_index);
		io_buffer = read_block_bitmap(inode->ext2, group_block->bg_block_bitmap, group_block_index);
		discard_prealloc_block(inode->ext2, group_block, inode, io_buffer);
		write_block_bitmap(inode->ext2, group_block->bg_block_bitmap, io_buffer, group_block_index, 0);
		write_group_block(inode->ext2, group_block_index, group_block, 0);
		if (lock)
		{
			sem_up(inode->ext2->sem);
		}
	}
}

static void inode_decrease_counter(t_inode* inode)
{
	inode->counter--;
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
		kfree(inode);
		ret = 0;
	}
	return ret;
}

//MAX REQUEST SIZE 64512 (i.e. 64K - 1K) BECAUSE ONE LBA SUPPORTED ONLY.
//IT NEEDED 1K OFFSET TO AVOID TO GO BEYOND 64K IN LBA.
//WRITE WORKS IN APPEND MODE ONLY.
int _read_write(t_ext2* ext2, int fd, void* buf, u32 count, u8 op_type, u8 is_dma)
{
	struct t_process_context* current_process_context;
	u32 i;
	u32 data_block;
	u32 block_addr;
	u32 block_addr_1;
	u32 block_addr_2;
	u32 block_offset;
	u32 first_block_offset;
	u32 first_inode_block;
	u32 first_data_offset;
	u32 last_inode_block;
	u32 inode_block;
	u32 lba;
	u32 indirect_lba;
	u32 sector_count;
	u32 byte_to_rw;
	u32 byte_rw;
	u32 byte_count;
	t_inode* inode = NULL;
	u32 inode_block_data; 
	char* iob_data_block = NULL;
	u32 second_block;
	u32 second_block_offset;
	t_llist* dma_lba_list = NULL;
	u32 last_lba = 0;
	u32 first_lba = 0;
	t_llist_node* first = NULL;
	t_llist_node* next = NULL;
	t_dma_lba* dma_lba = NULL;
	u32 dma_sector_count = 0;
	u32 buf_offset;
	u32 dma_buf_offset;
	u32 dma_lba_list_size = 0;
	unsigned char* dma_buffer = NULL;
	unsigned char* aligned_dma_buffer = NULL;
	unsigned char* phy_dma_buffer = NULL;
	u32 len;
	t_llist* block_disk_list = NULL;
	t_block_disk* block_disk = NULL;
	t_llist_node* sentinel_node = NULL;

	byte_rw = 0;
	byte_to_rw = count;
	CURRENT_PROCESS_CONTEXT(current_process_context)
	if (op_type == 1)
	{
		block_disk_list = new_dllist();
	}
	inode = hashtable_get(current_process_context->file_desc, fd);
	if (inode == NULL)
	{
		printk("inode not found !!!!! \n");
		panic();
		return -1;
	}
	if (is_dma == 1)
	{
		dma_buffer = kmalloc(DMA_BUFFER_SIZE);
//		for (i = 0; i < DMA_BUFFER_SIZE;i++)
//		{
//			dma_buffer[i] = 1;
//		}
		aligned_dma_buffer = ALIGN_TO_BOUNDARY((DMA_BUFFER_SIZE / 2), (u32) dma_buffer);
		phy_dma_buffer = FROM_VIRT_TO_PHY(ALIGN_TO_BOUNDARY((DMA_BUFFER_SIZE / 2), (u32) dma_buffer));
	}
	else
	{
		iob_data_block = kmalloc(BLOCK_SIZE);
	}

	first_inode_block = inode->file_offset / BLOCK_SIZE;
	first_data_offset = inode->file_offset % BLOCK_SIZE;
	if (inode->i_size < (inode->file_offset + (count - 1)) && op_type == 0)
	{
		last_inode_block = (inode->i_size / BLOCK_SIZE) - 1;
		byte_to_rw = inode->i_size - inode->file_offset;
	}
	else
	{
		last_inode_block = (inode->file_offset + (count - 1)) / BLOCK_SIZE;
		byte_to_rw = count;
	}
	
	for (i = first_inode_block; i <= last_inode_block; i++)
	{
		if (i > INDIRECT_0_LIMIT && i <= INDIRECT_1_LIMIT)
		{
			first_block_offset = (i - INDIRECT_0_LIMIT - 1);
			if (inode->i_block[12] == NULL)
			{
				block_addr = alloc_indirect_block(ext2, inode);
				if (block_addr == -1)
				{
					panic(); //only debug
					return -1;
				}
				inode->i_block[12] = block_addr;
				inode->i_blocks+= 2;
				inode->indirect_block_1 = indirect_block_init();
			}
			else
			{
				block_addr = inode->i_block[12];
			}
			if (inode->indirect_block_1 == NULL)
			{
				if(inode->i_block[12] != NULL)
				{
					inode->indirect_block_1 = indirect_block_init();
					indirect_lba = FROM_BLOCK_TO_LBA(inode->i_block[12]);
        				sector_count = BLOCK_SIZE/SECTOR_SIZE;
					READ(sector_count, indirect_lba, inode->indirect_block_1->block);
				}
			}
			if(inode->indirect_block_1->block[first_block_offset] == NULL)
			{
				data_block = alloc_block(ext2, inode, i, 1);
				inode->indirect_block_1->block[first_block_offset] = data_block;
				inode->i_blocks+= 2;
				block_disk = kmalloc(sizeof(t_block_disk));
				block_disk->lba = FROM_BLOCK_TO_LBA(block_addr);
				block_disk->data = inode->indirect_block_1->block;
				ll_append(block_disk_list, block_disk);
			}
			lba = FROM_BLOCK_TO_LBA(inode->indirect_block_1->block[first_block_offset]);
		}
		else if (i > INDIRECT_1_LIMIT  && i <= INDIRECT_2_LIMIT)
		{
			second_block = (i - INDIRECT_1_LIMIT - 1) / (BLOCK_SIZE / 4);
			second_block_offset = (i - INDIRECT_1_LIMIT - 1) % (BLOCK_SIZE /4);
			if (inode->i_block[13] == NULL)
			{
				block_addr_1 = alloc_indirect_block(ext2, inode);
				if (block_addr_1 == -1)
				{
					panic();//only debug
					return -1;
				}
				inode->i_block[13] = block_addr_1;
				inode->i_blocks+= 2;
				inode->indirect_block_2 = indirect_block_init();
				block_disk = kmalloc(sizeof(t_block_disk));
				block_disk->lba = FROM_BLOCK_TO_LBA(block_addr_1);
				block_disk->data = inode->indirect_block_2->block;
				ll_append(block_disk_list, block_disk);
			}
			if (inode->indirect_block_2 == NULL)
			{
				if(inode->i_block[13] != NULL)
				{
					inode->indirect_block_2 = indirect_block_init();
					indirect_lba = FROM_BLOCK_TO_LBA(inode->i_block[13]);
        				sector_count = BLOCK_SIZE / SECTOR_SIZE;
					READ(sector_count, indirect_lba, inode->indirect_block_2->block);
				}
			}
			if (inode->indirect_block_2->block_map[second_block] == NULL)
			{
				if(inode->indirect_block_2->block[second_block] != NULL)
				{
					inode->indirect_block_2->block_map[second_block] = indirect_block_init();
					indirect_lba = FROM_BLOCK_TO_LBA(inode->indirect_block_2->block[second_block]);
        				sector_count = BLOCK_SIZE / SECTOR_SIZE;
					READ(sector_count,indirect_lba, inode->indirect_block_2->block_map[second_block]->block);
					data_block = inode->indirect_block_2->block_map[second_block]->block[second_block_offset];
				}
				else
				{
					block_addr_2 = alloc_indirect_block(ext2, inode);
					inode->i_blocks+= 2;
					if (block_addr_2 == -1)
					{
						panic(); //only debug
						return -1;
					}
					inode->indirect_block_2->block[second_block] = block_addr_2;
					inode->indirect_block_2->block_map[second_block] = indirect_block_init();
					block_disk = kmalloc(sizeof(t_block_disk));
					block_disk->lba = FROM_BLOCK_TO_LBA(inode->i_block[13]);
					block_disk->data = inode->indirect_block_2->block;
					if (block_disk_list == NULL)
					{
						panic();
					}
					ll_append(block_disk_list, block_disk);
				}
			}
			if (inode->indirect_block_2->block_map[second_block]->block[second_block_offset] == NULL)
			{
				data_block = alloc_block(ext2, inode, i, 1);
				inode->i_blocks+= 2;
				inode->indirect_block_2->block_map[second_block]->block[second_block_offset] = data_block;
				block_disk = kmalloc(sizeof(t_block_disk));
				block_disk->lba = FROM_BLOCK_TO_LBA(inode->indirect_block_2->block[second_block]);
				block_disk->data = inode->indirect_block_2->block_map[second_block]->block;
				ll_append(block_disk_list, block_disk);
			}
			lba = FROM_BLOCK_TO_LBA(inode->indirect_block_2->block_map[second_block]->block[second_block_offset]);
		}
		else if ( i > INDIRECT_2_LIMIT  && i <= INDIRECT_3_LIMIT )
		{
			//TO DO
		}
		else
		{
			if(inode->i_block[i] == NULL)
			{
				data_block = alloc_block(ext2, inode, i, 1);
				inode->i_blocks+= 2;
				inode->i_block[i] = data_block;
			}
			lba = FROM_BLOCK_TO_LBA(inode->i_block[i]);
		}
		if(is_dma)
		{
			if(first_lba == 0 && i < last_inode_block)
			{
				first_lba = lba;
				last_lba = lba;
				dma_lba_list = new_dllist();
				dma_sector_count = (BLOCK_SIZE / SECTOR_SIZE);
			}
			else if(first_lba == 0 && i == last_inode_block)
			{
				dma_lba_list = new_dllist();
				dma_sector_count = (BLOCK_SIZE / SECTOR_SIZE);
				dma_lba_list_size = 1;
				dma_lba = kmalloc(sizeof(t_dma_lba));
				dma_lba->lba = lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list, dma_lba);
			}
			else if(lba == (last_lba + (BLOCK_SIZE / SECTOR_SIZE)) && i < last_inode_block)
			{
				dma_sector_count += (BLOCK_SIZE / SECTOR_SIZE);
				last_lba = lba;
			}
			else if (i == last_inode_block && lba == (last_lba + (BLOCK_SIZE / SECTOR_SIZE)))
			{
				dma_sector_count += (BLOCK_SIZE / SECTOR_SIZE);
				dma_lba_list_size++;
				dma_lba = kmalloc(sizeof(t_dma_lba));		
				dma_lba->lba = first_lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list, dma_lba);
			}
			else if (i == last_inode_block && lba != (last_lba + (BLOCK_SIZE / SECTOR_SIZE)))
			{
				dma_lba_list_size++;
				dma_lba = kmalloc(sizeof(t_dma_lba));
				dma_lba->lba = first_lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list, dma_lba);

				dma_lba_list_size++;
				dma_sector_count = (BLOCK_SIZE / SECTOR_SIZE);
				dma_lba = kmalloc(sizeof(t_dma_lba));
				dma_lba->lba = lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list, dma_lba);
			}
			else
			{
				dma_lba_list_size++;
				dma_lba = kmalloc(sizeof(t_dma_lba));		
				dma_lba->lba = first_lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list, dma_lba);
				dma_sector_count = (BLOCK_SIZE / SECTOR_SIZE);
				last_lba = lba;
				first_lba = lba;
			}
		}
		else
		{
			if (byte_to_rw >= BLOCK_SIZE)
			{
				if (first_data_offset == 0 || i > first_inode_block)
				{
					byte_count = BLOCK_SIZE;
					byte_to_rw -= BLOCK_SIZE;
				}
				else
				{
					byte_count = BLOCK_SIZE - first_data_offset;
					byte_to_rw -= (BLOCK_SIZE - first_data_offset);
				}	
			}
			else
			{
				if (first_data_offset == 0 || i > first_inode_block)
				{
					byte_count = byte_to_rw;
					byte_to_rw = 0;
				}
				else 
				{
					if (BLOCK_SIZE - first_data_offset > byte_to_rw)
					{
						byte_count = byte_to_rw;
						byte_to_rw = 0;
					}
					else
					{
						byte_count = BLOCK_SIZE - first_data_offset;
						byte_to_rw -= (BLOCK_SIZE - first_data_offset);
					}	
				}
			}
        		sector_count = BLOCK_SIZE/SECTOR_SIZE;
			if (op_type == 0)
			{
				READ(sector_count,lba,iob_data_block);
				kmemcpy(buf,iob_data_block + first_data_offset,byte_count);
			}
			else if (op_type == 1)
			{
				if (i == 0 && first_data_offset > 0)
				{
					READ(sector_count, lba, iob_data_block);
				}
				kmemcpy(iob_data_block + first_data_offset, buf, byte_count);
				WRITE(sector_count, lba, iob_data_block);
			}
			inode->file_offset += byte_count;
			buf += byte_count;
			byte_rw += byte_count;
			first_data_offset = 0;
		}
	}
	
	//A bit convoluted code.Is it possible to do better?
	if (is_dma)
	{
		buf_offset = 0;
		byte_count = 0;
		len = 0;

		first = ll_first(dma_lba_list);
		next = first;
		for (i = 0; i < dma_lba_list_size; i++)
		{
			dma_buf_offset = 0;
			dma_lba = next->val;
			len = dma_lba->sector_count * SECTOR_SIZE;
			if (i == 0 && dma_lba_list_size > 1)
			{
				byte_count = len - first_data_offset;
				dma_buf_offset = first_data_offset;
			}
			else if (i == 0 && dma_lba_list_size == 1)
			{
				byte_count = byte_to_rw;
				dma_buf_offset = first_data_offset;
			}
			else if (i == (dma_lba_list_size - 1) && (len + byte_rw > byte_to_rw) && i > 0)
			{
				byte_count = byte_to_rw - byte_rw;
			}
			else
			{
				byte_count = dma_lba->sector_count * SECTOR_SIZE;	
			}
			if (op_type == 0)
			{
				READ_DMA(dma_lba->sector_count, dma_lba->lba, phy_dma_buffer);
				kmemcpy(buf + buf_offset, aligned_dma_buffer + dma_buf_offset, byte_count);
			}
			else if (op_type == 1)
			{
				if (i == 0 && first_data_offset > 0)
				{
					READ((BLOCK_SIZE/SECTOR_SIZE),dma_lba->lba,aligned_dma_buffer);
				}
				kmemcpy(aligned_dma_buffer + dma_buf_offset, buf + buf_offset, byte_count);
				WRITE_DMA(dma_lba->sector_count, dma_lba->lba, phy_dma_buffer);
			}
			buf_offset += byte_count;
			next = ll_next(next);
		 	byte_rw += byte_count;
		}
		kfree(dma_buffer);
		inode->file_offset += byte_rw;
		free_llist(dma_lba_list);
	}
	else
	{
		kfree(iob_data_block);
	}
	if (op_type == 1)
	{
		sentinel_node = ll_sentinel(block_disk_list);
		next = ll_first(block_disk_list);
		while(next != sentinel_node)
		{
			block_disk = next->val;
			WRITE((BLOCK_SIZE / SECTOR_SIZE), block_disk->lba, block_disk->data);
			next = ll_next(next);
		}
		free_llist(block_disk_list);
	}
	if (op_type == 1)
	{
		if (inode->file_offset > inode->i_size)
		{
			inode->i_size += inode->file_offset - inode->i_size;
		}
		//write_inode(ext2, inode, 1);
	}
	return byte_rw;
}

//MAX REQUEST SIZE 64512 (i.e. 64K - 1K) BECAUSE ONE LBA SUPPORTED ONLY.
//IT NEEDED 1K OFFSET TO AVOID TO GO BEYOND 64K IN LBA
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
	char* iob_data_block = NULL;
	u32 second_block;
	u32 second_block_offset;
	t_llist* dma_lba_list = NULL;
	u32 last_lba = 0;
	u32 first_lba = 0;
	t_llist_node* first = NULL;
	t_llist_node* next = NULL;
	t_dma_lba* dma_lba = NULL;
	u32 dma_sector_count = 0;
	u32 buf_offset;
	u32 dma_buf_offset;
	u32 dma_lba_list_size = 0;
	unsigned char* dma_buffer = NULL;
	unsigned char* aligned_dma_buffer = NULL;
	unsigned char* phy_dma_buffer = NULL;
	u32 len;
	
	byte_read = 0;
	byte_to_read = count;
	CURRENT_PROCESS_CONTEXT(current_process_context)
	inode = hashtable_get(current_process_context->file_desc,fd);
	if (inode == NULL)
	{
		return -1;
	}
	if (is_dma == 1)
	{
		dma_buffer = kmalloc(DMA_BUFFER_SIZE);
		aligned_dma_buffer = ALIGN_TO_BOUNDARY((DMA_BUFFER_SIZE / 2),(u32)dma_buffer);
		phy_dma_buffer = FROM_VIRT_TO_PHY(ALIGN_TO_BOUNDARY((DMA_BUFFER_SIZE / 2),(u32) dma_buffer));
	}
	else
	{
		iob_data_block = kmalloc(BLOCK_SIZE);
	}

	first_inode_block = inode->file_offset / BLOCK_SIZE;
	first_data_offset = inode->file_offset % BLOCK_SIZE;
	if (inode->i_size < (inode->file_offset + (count - 1)))
	{
		last_inode_block = (inode->i_size / BLOCK_SIZE) - 1;
		byte_to_read = inode->i_size - inode->file_offset;
	}
	else
	{
		last_inode_block = (inode->file_offset + (count - 1)) / BLOCK_SIZE;
		byte_to_read = count;
	}
	
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
			READ_DWORD(&inode->indirect_block_1->block[(i - INDIRECT_0_LIMIT - 1)],inode_block_data);
			lba = FROM_BLOCK_TO_LBA(inode_block_data);
		}
		else if (i > INDIRECT_1_LIMIT  && i <= INDIRECT_2_LIMIT)
		{
			second_block = (i - INDIRECT_1_LIMIT - 1) / (BLOCK_SIZE / 4);
			second_block_offset = (i - INDIRECT_1_LIMIT - 1) % (BLOCK_SIZE /4);
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
		if(is_dma)
		{
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
				dma_lba->lba = first_lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list,dma_lba);
			}
			else if (i == last_inode_block && lba != (last_lba + (BLOCK_SIZE/SECTOR_SIZE)))
			{
				dma_lba_list_size++;
				dma_lba = kmalloc(sizeof(t_dma_lba));
				dma_lba->lba = first_lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list,dma_lba);

				dma_lba_list_size++;
				dma_sector_count = (BLOCK_SIZE/SECTOR_SIZE);
				dma_lba = kmalloc(sizeof(t_dma_lba));
				dma_lba->lba = lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list,dma_lba);
			}
			else
			{
				dma_lba_list_size++;
				dma_lba = kmalloc(sizeof(t_dma_lba));		
				dma_lba->lba = first_lba;
				dma_lba->sector_count = dma_sector_count;
				ll_append(dma_lba_list,dma_lba);
				dma_sector_count = (BLOCK_SIZE/SECTOR_SIZE);
				last_lba = lba;
				first_lba = lba;
			}
		}
		else
		{
			if (byte_to_read >= BLOCK_SIZE)
			{
				if (first_data_offset == 0 || i > first_inode_block)
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
				if (first_data_offset == 0 || i > first_inode_block)
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
        		sector_count = BLOCK_SIZE/SECTOR_SIZE;
			READ(sector_count,lba,iob_data_block);
			kmemcpy(buf,iob_data_block + first_data_offset,byte_count);
			inode->file_offset += byte_count;
			buf += byte_count;
			byte_read += byte_count;
			first_data_offset = 0;
		}
	}
	//A bit convoluted code.Is it possible to do better?
	if (is_dma)
	{
		buf_offset = 0;
		byte_count = 0;
		len = 0;

		first = ll_first(dma_lba_list);
		next = first;
		for (i = 0; i < dma_lba_list_size;i++)
		{

			dma_buf_offset = 0;
			dma_lba = next->val;
			len = dma_lba->sector_count * SECTOR_SIZE;
			if (i == 0 && dma_lba_list_size > 1)
			{
				byte_count = len - first_data_offset;
				dma_buf_offset = first_data_offset;
			}
			else if (i == 0 && dma_lba_list_size == 1)
			{
				byte_count = byte_to_read;
				dma_buf_offset = first_data_offset;
			}
			else if (i == (dma_lba_list_size - 1) && (len + byte_read > byte_to_read) && i > 0)
			{
				byte_count = byte_to_read - byte_read;
			}
			else
			{
				byte_count = dma_lba->sector_count * SECTOR_SIZE;	
			}
			READ_DMA(dma_lba->sector_count,dma_lba->lba,phy_dma_buffer);
			kmemcpy(buf + buf_offset,aligned_dma_buffer + dma_buf_offset,byte_count);
			buf_offset += byte_count;
			next = ll_next(next);
			byte_read += byte_count;
		}
		kfree(dma_buffer);
		inode->file_offset += byte_read;
		free_llist(dma_lba_list);
	}
	else
	{
		kfree(iob_data_block);
	}
	return byte_read;
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
	return offset;
}

int _rm(t_ext2* ext2,char* fullpath)
{
	int ret = -1;
	t_inode* inode = NULL;
	t_inode* inode_dir = NULL;
	char path[NAME_MAX];
	char filename[NAME_MAX];

	//inode = kmalloc(sizeof(t_inode));
	//inode_dir = kmalloc(sizeof(t_inode));
	find_parent_path_and_filename(fullpath, path, filename);
	inode = lookup_inode(fullpath, ext2);
	if (inode == NULL)
	{
		goto EXIT;
	}
	inode_dir = lookup_inode(path, ext2);
	if (inode_dir == NULL)
	{
		goto EXIT;
	}
	if (inode->i_mode  & 0x4000)
	{
		del_full_dir(ext2, inode, inode_dir, 1);
		inode_dir->i_links_count--;
		write_inode(ext2, inode_dir, 1, 1);
	}
	ret = del_dir_entry(ext2, inode_dir, inode);
	sync_fs(ext2);
EXIT:
	//kfree(inode);
	//kfree(inode_dir);
	return ret;
}


//HO TOLTO PARENT_DIR DA LOOKUP_INODE DA RIVEDERE  _MKDIR
int _mkdir(t_ext2* ext2,const char* full_path)
{
	static int counter = 0;
	int ret;
	char parent_path[NAME_MAX];
	char file_name[NAME_MAX];
	t_inode* inode = NULL;
	t_inode* inode_parent_dir = NULL;
	char* iob_dir = NULL;
	u32 block_num;
	
	ret = 0;
	iob_dir = kmalloc(BLOCK_SIZE);
	inode = inode_init(ext2);
	//inode->parent_dir = inode_init(ext2);
	find_parent_path_and_filename(full_path, parent_path, file_name);
	inode_parent_dir = lookup_inode(parent_path, ext2);
	inode->i_number = alloc_inode(inode_parent_dir, 1, ext2);
	if (inode->i_number == -1)
	{
		ret = -1;
		goto EXIT;
	}
	block_num = alloc_block(ext2, inode, 0, 1);
	if (block_num == -1)
	{
		//free_inode(inode, ext2);
		inode_free(inode);
		ret = -1;
		goto EXIT;
	}
	inode->i_block[0] = block_num;
	inode->i_mode = 0x41FF;
	inode->i_size = BLOCK_SIZE;
	inode->i_links_count = 2;
	inode->i_blocks = 2;
	if (inode->i_number == 49)
	{
		panic();
	}
	write_inode(ext2, inode, 1, 1);

	kfillmem(iob_dir, 0, BLOCK_SIZE);
	iob_dir[0] = inode->i_number & 0xFF;
	iob_dir[1] = (inode->i_number & 0xFF00) >> 8;
	iob_dir[2] = (inode->i_number & 0xFF00) >> 16;
	iob_dir[3] = (inode->i_number & 0xFF00) >> 24;
	iob_dir[4] = 0xC;
	iob_dir[5] = 0;
	iob_dir[6] = 1;
	iob_dir[7] = 2;
	iob_dir[8] = '.';
	iob_dir[9] = '\0';
	iob_dir[10] = '\0';
	iob_dir[11] = '\0';
	
	iob_dir[12] = inode_parent_dir->i_number & 0xFF;
	iob_dir[13] = (inode_parent_dir->i_number & 0xFF00) >> 8;
    iob_dir[14] = (inode_parent_dir->i_number & 0xFF00) >> 16;
    iob_dir[15] = (inode_parent_dir->i_number & 0xFF00) >> 24;
	iob_dir[16] = 0xF4;
	iob_dir[17] = 0x3;
	iob_dir[18] = 2;
	iob_dir[19] = 2;
	iob_dir[20] = '.';
	iob_dir[21] = '.';
	iob_dir[22] = '\0';
	iob_dir[23] = '\0';
	WRITE((BLOCK_SIZE / SECTOR_SIZE), FROM_BLOCK_TO_LBA(block_num), iob_dir);

	ret = add_entry_to_dir(file_name, inode_parent_dir, ext2,inode->i_number, DIRECTORY);
	if (ret == -1)
	{
		inode_prealloc_block_free(inode, 1);
		free_inode(inode, ext2, 1);
		ret = -1;
		goto EXIT;
	}
	//inode_prealloc_block_free(inode, 1);
	//inode_free(inode);
EXIT:
	sync_fs(ext2);
	kfree(iob_dir);
	return ret;
}

int _chdir(t_ext2* ext2,char* path)
{
	u32 current_dir_inode;
	t_inode* inode = NULL;
	struct t_process_context* current_process_context = NULL;

	//inode = inode_init(ext2);
	CURRENT_PROCESS_CONTEXT(current_process_context)
	inode = lookup_inode(path,ext2);
	if (inode == NULL)
	{
		current_process_context->current_dir_inode_number = inode->i_number;
	}
}

int _stat(t_ext2* ext2, char* pathname, t_stat* stat)
{
	t_inode* inode = NULL;

	//inode = inode_init(ext2);
	inode = lookup_inode(pathname, ext2);		
	if (inode == NULL)
	{
		return -1;
	}
	stat->st_ino = inode->i_number;
	stat->st_mode = inode->i_mode;
	stat->st_uid = inode->i_uid;
	stat->st_gid = inode->i_gid;
	stat->st_size = inode->i_size;
	stat->st_atime = inode->i_atime;
	stat->st_mtime = inode->i_mtime;
	stat->st_ctime = inode->i_ctime;
	return 0;	
}

int _mount(char* pathname,t_ext2* ext2)
{
	t_mount_point* mount_point = NULL;
	t_inode* inode = NULL;

	//inode = inode_init(ext2);
	inode = lookup_inode(pathname,ext2);		
	if (inode == NULL)
	{
		return -1;
	}
	mount_point = kmalloc(sizeof(t_mount_point));
	mount_point->inode = inode;
	mount_point->ext2 = ext2;
	hashtable_put(system.mount_map, inode->i_number, mount_point);
	return 0;
}

int _umount(int inode_number)
{
	t_mount_point* mount_point = NULL;

	mount_point = hashtable_remove(system.mount_map, inode_number);
	if (mount_point == NULL)
	{
		return -1;
	}
	kfree(mount_point->inode);
	kfree(mount_point);
	return 0;
}
