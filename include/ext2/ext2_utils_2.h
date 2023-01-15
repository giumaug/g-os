static char* read_inode_bitmap(t_ext2* ext2, u32 bg_inode_bitmap, u32 group_block_index)
{
	u32 lba;
	u32 sector_count;
	char* io_buffer = NULL;

	if (ext2->superblock->group_inode_bitmap_list[group_block_index] == NULL)
	{
		io_buffer = aligned_kmalloc(BLOCK_SIZE, 16);
       	lba = ext2->partition_start_sector + (bg_inode_bitmap * BLOCK_SIZE / SECTOR_SIZE);
        sector_count = BLOCK_SIZE / SECTOR_SIZE;
		READ(sector_count, lba, io_buffer);
		ext2->superblock->group_inode_bitmap_list[group_block_index] = io_buffer;
	}
	else
	{
		system.read_bitmap_count++;
		io_buffer = ext2->superblock->group_inode_bitmap_list[group_block_index];
	}
	return io_buffer;
}

static void write_inode_bitmap(t_ext2* ext2, u32 bg_inode_bitmap, void* io_buffer, u32 group_block_index, u8 store_on_disk)
{
	u32 lba;
	u32 sector_count;

	if (store_on_disk == 0)
	{
		ext2->superblock->group_inode_bitmap_list_status[group_block_index] = 1;
	}
	else
	{
		ext2->superblock->group_inode_bitmap_list_status[group_block_index] = 0;	
        lba = ext2->partition_start_sector + (bg_inode_bitmap * BLOCK_SIZE / SECTOR_SIZE);
        sector_count = BLOCK_SIZE / SECTOR_SIZE;
		WRITE(sector_count, lba, io_buffer);
	}
}

static char* read_block_bitmap(t_ext2* ext2, u32 bg_block_bitmap, u32 group_block_index)
{
	u32 lba;
	u32 sector_count;
	char* io_buffer = NULL;

	if (ext2->superblock->group_block_bitmap_list[group_block_index] == NULL)
	{
		io_buffer = aligned_kmalloc(BLOCK_SIZE, 16);
       	lba = ext2->partition_start_sector + (bg_block_bitmap * BLOCK_SIZE / SECTOR_SIZE);
        sector_count = BLOCK_SIZE / SECTOR_SIZE;
		READ(sector_count, lba, io_buffer);
		ext2->superblock->group_block_bitmap_list[group_block_index] = io_buffer;
	}
	else
	{
		system.read_bitmap_count++;
		io_buffer = ext2->superblock->group_block_bitmap_list[group_block_index];
	}
	return io_buffer;
}

static void write_block_bitmap(t_ext2* ext2, u32 bg_block_bitmap, void* io_buffer, u32 group_block_index, u8 store_on_disk)
{
	u32 lba;
	u32 sector_count;

	if (store_on_disk == 0)
	{
		ext2->superblock->group_block_bitmap_list_status[group_block_index] = 1;
	}
	else
	{
		ext2->superblock->group_block_bitmap_list_status[group_block_index] = 0;	
        lba = ext2->partition_start_sector + (bg_block_bitmap * BLOCK_SIZE / SECTOR_SIZE);
        sector_count = BLOCK_SIZE / SECTOR_SIZE;
		WRITE(sector_count, lba, io_buffer);
	}
}

static int alloc_indirect_block(t_ext2* ext2,t_inode* i_node)
{
	u32 group_block_index;
	u32 indirect_block;
	s32 rel_block;
	int ret;
	t_group_block* group_block = NULL;
	void* io_buffer = NULL;
	int i;
	u32 tot_fs_block;
	u32 tot_fs_group;
	u32 blocks_per_group;

	tot_fs_block = ext2->superblock->s_blocks_count;
	blocks_per_group = ext2->superblock->s_blocks_per_group;
	group_block_index = (i_node->i_number - 1) / ext2->superblock->s_inodes_per_group;
	tot_fs_group = TOT_FS_GROUP(ext2->superblock->s_blocks_count, ext2->superblock->s_blocks_per_group);
	ret = -1;
	group_block = read_group_block(ext2,group_block_index);
	io_buffer = read_block_bitmap(ext2,group_block->bg_block_bitmap,group_block_index);
	rel_block = find_free_block(io_buffer, group_block_index, tot_fs_block, blocks_per_group, ext2);
	if (rel_block != -1)
	{
		indirect_block = rel_block; 
		write_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer,group_block_index, 0);
		group_block->bg_free_blocks_count--;
		write_group_block(ext2,group_block_index,group_block, 0);
		ext2->superblock->s_free_blocks_count--;

		ret = indirect_block;
	}
	else 
	{
		for(i = 0; i < tot_fs_group; i++)
		{
			if (i != group_block_index)
			{
				group_block = read_group_block(ext2,i);
				io_buffer = read_block_bitmap(ext2,group_block->bg_block_bitmap,i);
				rel_block = find_free_block(io_buffer, i, tot_fs_block, blocks_per_group, ext2);
				if (rel_block != -1)
				{
					indirect_block = ABSOLUTE_BLOCK_ADDRESS(group_block_index,rel_block);
					write_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer, i, 0);
					group_block->bg_free_blocks_count--;
					write_group_block(ext2,i,group_block, 0);
					ret = indirect_block;
					ext2->superblock->s_free_blocks_count--;
					//write_superblock(ext2);
					break;
				}
			}
		}
	}
	return ret;	       
}

static void free_indirect_block(t_ext2* ext2,t_inode* i_node)
{
	u32 buffer_byte;
	u32 byte_bit;
	u32 group_block_index;
	u32 relative_block_address;
	t_group_block* group_block;
	char* io_buffer = NULL;

	group_block_index = i_node->i_block[12] / ext2->superblock->s_blocks_per_group;
	relative_block_address = i_node->i_block[12] % ext2->superblock->s_blocks_per_group;
	group_block = read_group_block(ext2, group_block_index);
	io_buffer = read_block_bitmap(ext2, group_block->bg_block_bitmap,group_block_index);
	buffer_byte = RELATIVE_BITMAP_BYTE(relative_block_address, ext2->superblock->s_blocks_per_group);
	byte_bit = RELATIVE_BITMAP_BIT(relative_block_address, ext2->superblock->s_blocks_per_group);
	io_buffer[buffer_byte] &= (255 &  ~ (2 >> byte_bit));
	write_block_bitmap(ext2, group_block->bg_block_bitmap, io_buffer,group_block_index, 0);
	write_group_block(ext2, group_block_index, group_block, 0);
}

static u32 read_indirect_block(t_ext2* ext2,t_inode* inode,u32 key)
{
	u32 block_addr;
	u32 second_block;
	u32 second_block_offset;

	if (key > INDIRECT_0_LIMIT && key <= INDIRECT_1_LIMIT)
	{
		block_addr = inode->indirect_block_1->block[key - INDIRECT_0_LIMIT - 1];
	}
        else if (key > INDIRECT_1_LIMIT  && key <= INDIRECT_2_LIMIT)
	{
		second_block = (key - INDIRECT_1_LIMIT - 1) / (BLOCK_SIZE / 4);
		second_block_offset = (key - INDIRECT_1_LIMIT - 1) % (BLOCK_SIZE / 4);
		block_addr = inode->indirect_block_2->block_map[second_block]->block[second_block_offset];
	}
	else if (key > INDIRECT_2_LIMIT  && key <= INDIRECT_3_LIMIT )
	{
		// TO DO
	}
	else
	{
		block_addr = inode->i_block[key];
	}
	return block_addr;
}

void static read_superblock(t_ext2* ext2)
{
    char* io_buffer = NULL;
	t_superblock* superblock = NULL;
	int i=0;
	
    io_buffer = aligned_kmalloc(BLOCK_SIZE, 16);
    P_READ(2, (2 + ext2->partition_start_sector), io_buffer);	
	superblock = ext2->superblock;
       
    //u32
    READ_DWORD(&io_buffer[0], superblock->s_inodes_count);
    READ_DWORD(&io_buffer[4], superblock->s_blocks_count);
    READ_DWORD(&io_buffer[8], superblock->s_r_blocks_count);
    READ_DWORD(&io_buffer[12], superblock->s_free_blocks_count);
    READ_DWORD(&io_buffer[16], superblock->s_free_inodes_count);
    READ_DWORD(&io_buffer[20], superblock->s_first_data_block);
    READ_DWORD(&io_buffer[24], superblock->s_log_block_size);
    READ_DWORD(&io_buffer[28], superblock->s_log_frag_size);
    READ_DWORD(&io_buffer[32], superblock->s_blocks_per_group);
    READ_DWORD(&io_buffer[36], superblock->s_frags_per_group);
    READ_DWORD(&io_buffer[40], superblock->s_inodes_per_group);
    READ_DWORD(&io_buffer[44], superblock->s_mtime);
    READ_DWORD(&io_buffer[48], superblock->s_wtime);
    //u16
    READ_WORD(&io_buffer[52], superblock->s_mnt_count);
    READ_WORD(&io_buffer[54], superblock->s_max_mnt_count);
    READ_WORD(&io_buffer[56], superblock->s_magic);
    READ_WORD(&io_buffer[58], superblock->s_state);
    READ_WORD(&io_buffer[60], superblock->s_errors);
    READ_WORD(&io_buffer[62], superblock->s_minor_rev_level);
    //u32
    READ_DWORD(&io_buffer[64], superblock->s_lastcheck);          
    READ_DWORD(&io_buffer[68], superblock->s_checkinterval);
    READ_DWORD(&io_buffer[72], superblock->s_creator_os);
    READ_DWORD(&io_buffer[76], superblock->s_rev_level);
    //u16
    READ_WORD(&io_buffer[80], superblock->s_def_resuid);            
    READ_WORD(&io_buffer[82], superblock->s_def_resgid);
    //u32
    READ_DWORD(&io_buffer[84], superblock->s_first_ino);                      
    //u16
    READ_WORD(&io_buffer[88], superblock->s_inode_size);
    READ_WORD(&io_buffer[90], superblock->s_block_group_nr);
    //u32
    READ_DWORD(&io_buffer[92], superblock->s_feature_compat);
    READ_DWORD(&io_buffer[96], superblock->s_feature_incompat);
    READ_DWORD(&io_buffer[100], superblock->s_feature_ro_compat);
    //u8[16]
    kmemcpy(&superblock->s_uuid, &io_buffer[104],16);  
    //s8[16]
    kmemcpy(&superblock->s_volume_name, &io_buffer[120],16);
    //u64
    kmemcpy(&superblock->s_last_mounted, &io_buffer[136],64);
    //u32
    READ_DWORD(&io_buffer[200], superblock->s_algorithm_usage_bitmap);
    //u8
    READ_BYTE(&io_buffer[204], superblock->s_prealloc_blocks);      
    READ_BYTE(&io_buffer[205], superblock->s_prealloc_dir_blocks);
    //u16
    READ_WORD(&io_buffer[206], superblock->s_padding1);
    //u32[204]
    kmemcpy(&superblock->s_reserved, &io_buffer[208], 816);
	aligned_kfree(io_buffer);
}

struct s_ext2;
void static write_superblock(struct s_ext2* ext2)
{
	char* io_buffer = NULL;
	t_superblock* superblock = NULL;
	
   	io_buffer=aligned_kmalloc(BLOCK_SIZE, 16);
	superblock=ext2->superblock;
   
   	//u32
   	WRITE_DWORD(superblock->s_inodes_count, &io_buffer[0]);
  	WRITE_DWORD(superblock->s_blocks_count, &io_buffer[4]);
   	WRITE_DWORD(superblock->s_r_blocks_count, &io_buffer[8]);
   	WRITE_DWORD(superblock->s_free_blocks_count, &io_buffer[12]);
   	WRITE_DWORD(superblock->s_free_inodes_count, &io_buffer[16]);
   	WRITE_DWORD(superblock->s_first_data_block, &io_buffer[20]);
   	WRITE_DWORD(superblock->s_log_block_size, &io_buffer[24]);
   	WRITE_DWORD(superblock->s_log_frag_size, &io_buffer[28]);
   	WRITE_DWORD(superblock->s_blocks_per_group, &io_buffer[32]);
   	WRITE_DWORD(superblock->s_frags_per_group, &io_buffer[36]);
   	WRITE_DWORD(superblock->s_inodes_per_group, &io_buffer[40]);
   	WRITE_DWORD(superblock->s_mtime, &io_buffer[44]);
   	WRITE_DWORD(superblock->s_wtime, &io_buffer[48]);
   	//u16
   	WRITE_WORD(superblock->s_mnt_count, &io_buffer[52]);    
   	WRITE_WORD(superblock->s_max_mnt_count, &io_buffer[54]);
   	WRITE_WORD(superblock->s_magic, &io_buffer[56]);
   	WRITE_WORD(superblock->s_state, &io_buffer[58]);
   	WRITE_WORD(superblock->s_errors, &io_buffer[60]);
   	WRITE_WORD(superblock->s_minor_rev_level, &io_buffer[62]);
   	//u32
   	WRITE_DWORD(superblock->s_lastcheck, &io_buffer[64]);        
  	WRITE_DWORD(superblock->s_checkinterval, &io_buffer[68]);
   	WRITE_DWORD(superblock->s_creator_os, &io_buffer[72]);
  	WRITE_DWORD(superblock->s_rev_level, &io_buffer[76]);
   	//u16
   	WRITE_WORD(superblock->s_def_resuid, &io_buffer[80]);          
   	WRITE_WORD(superblock->s_def_resgid, &io_buffer[82]);
  	//u32
   	WRITE_DWORD(superblock->s_first_ino, &io_buffer[84]);                    
   	//u16
   	WRITE_WORD(superblock->s_inode_size, &io_buffer[88]);
   	WRITE_WORD(superblock->s_block_group_nr, &io_buffer[90]);
   	//u32
   	WRITE_DWORD(superblock->s_feature_compat, &io_buffer[92]);
   	WRITE_DWORD(superblock->s_feature_incompat, &io_buffer[96]);
   	WRITE_DWORD(superblock->s_feature_ro_compat, &io_buffer[100]);
   	//u8[16]
	kmemcpy(&io_buffer[104], &superblock->s_uuid,16);
    //s8[16]
	kmemcpy(&io_buffer[120], &superblock->s_volume_name,16);
    //u64
	kmemcpy(&io_buffer[136], &superblock->s_last_mounted,64);
    //u32
    WRITE_DWORD(superblock->s_algorithm_usage_bitmap, &io_buffer[200]);
    //u8
    WRITE_BYTE(superblock->s_prealloc_blocks, &io_buffer[204]);   
    WRITE_BYTE(superblock->s_prealloc_dir_blocks, &io_buffer[205]);
    //u16
    WRITE_WORD(superblock->s_padding1, &io_buffer[206]);
    //u32[204]
	kmemcpy(&io_buffer[208], &superblock->s_reserved, 816);

	WRITE(2, (2 + ext2->partition_start_sector), io_buffer);
	aligned_kfree(io_buffer);
}

static void write_group_block(t_ext2* ext2, u32 group_block_number, t_group_block* group_block, u8 store_on_disk)
{
	u32 block_number;
	u32 sector_number;
	u32 sector_offset;
	u32 lba;
	char* io_buffer = NULL;

	if (store_on_disk == 0)
	{
		ext2->superblock->group_block_list_status[group_block_number] = 1;
	}
	else 
	{
		ext2->superblock->group_block_list_status[group_block_number] = 0;
		//block_number start from 1
		sector_number = 32 * group_block_number / SECTOR_SIZE;
		sector_offset = (32 * group_block_number) % SECTOR_SIZE;
		lba=(2 * BLOCK_SIZE) / SECTOR_SIZE + ext2->partition_start_sector + sector_number;
		io_buffer = aligned_kmalloc(512, 16);
		READ(1, lba, io_buffer);

		//u32
		WRITE_DWORD(group_block->bg_block_bitmap, &io_buffer[0 + sector_offset]);
		//u32
		WRITE_DWORD(group_block->bg_inode_bitmap, &io_buffer[4 + sector_offset]);
		//u32
		WRITE_DWORD(group_block->bg_inode_table, &io_buffer[8 + sector_offset]);
		//u16
		WRITE_WORD(group_block->bg_free_blocks_count, &io_buffer[12 + sector_offset]);
		//u16
		WRITE_WORD(group_block->bg_free_inodes_count, &io_buffer[14 + sector_offset]);
		//u16
		WRITE_WORD(group_block->bg_used_dirs_count, &io_buffer[16 + sector_offset]);
		//u16
		WRITE_WORD(group_block->bg_pad, &io_buffer[18 + sector_offset]);
		//u32[3]
		kmemcpy(&io_buffer[20 + sector_offset], group_block->bg_reserved,3);   
	
		WRITE(1,lba,io_buffer);
		aligned_kfree(io_buffer);
	}
}

//read_group_block(t_ext2 *ext2,u32 group_block_number,t_group_block* group_block)
t_group_block* read_group_block(t_ext2 *ext2,u32 group_block_number)
{
	u32 sector_number;
	u32 sector_offset;
	u32 lba;
	char* io_buffer = NULL;
	t_group_block* group_block = NULL;

	if (ext2->superblock->group_block_list[group_block_number] == NULL)
	{
		sector_number = 32 * group_block_number / SECTOR_SIZE;
		sector_offset = (32 * group_block_number) % SECTOR_SIZE;
		lba = (2 * BLOCK_SIZE) / SECTOR_SIZE + ext2->partition_start_sector + sector_number;
		io_buffer = aligned_kmalloc(512, 16);
		group_block = kmalloc(sizeof (t_group_block));
		ext2->superblock->group_block_list[group_block_number] = group_block;

		READ(1, lba, io_buffer);
		//u32
		READ_DWORD(&io_buffer[0 + sector_offset], group_block->bg_block_bitmap);
		//u32
		READ_DWORD(&io_buffer[4 + sector_offset], group_block->bg_inode_bitmap);
		//u32
		READ_DWORD(&io_buffer[8 + sector_offset], group_block->bg_inode_table);
		//u16
		READ_WORD(&io_buffer[12 + sector_offset], group_block->bg_free_blocks_count);
		//u16
		READ_WORD(&io_buffer[14 + sector_offset], group_block->bg_free_inodes_count);
		//u16
		READ_WORD(&io_buffer[16 + sector_offset], group_block->bg_used_dirs_count);
		//u16
		READ_WORD(&io_buffer[18 + sector_offset], group_block->bg_pad);
		//u32[3]
		kmemcpy(group_block->bg_reserved, &io_buffer[20 + sector_offset], 3);
		aligned_kfree(io_buffer);
	}
	else
	{
		system.read_block_count++;
		group_block = ext2->superblock->group_block_list[group_block_number];
	}
	return group_block;
}

//struct s_inode;
static t_inode* read_inode(t_ext2* ext2, u32 i_number)
{
	t_inode* inode = NULL;
	u32 group_number;
	u32 group_offset;
	u32 inode_table_offset;
	u32 inode_offset;
	t_group_block* group_block = NULL;
	u32 lba;
	u32 sector_count;
	char* io_buffer = NULL;
	u32 group_i_number;

	inode = get_inode_cache(ext2->superblock->inode_cache, i_number);
	if (inode  == NULL)
	{
		inode = inode_init(ext2);
		inode->status = 0;
		inode->i_number = i_number;
		io_buffer = aligned_kmalloc(BLOCK_SIZE, 16);
		group_number = (i_number - 1) / ext2->superblock->s_inodes_per_group; 
		group_block = read_group_block(ext2, group_number);
		group_i_number = i_number - (group_number * ext2->superblock->s_inodes_per_group) - 1;
		inode_table_offset = group_i_number * 128 / BLOCK_SIZE;
		inode_offset=(i_number - 1) * 128 % BLOCK_SIZE;
		lba = ext2->partition_start_sector + (inode_table_offset + group_block->bg_inode_table) * (BLOCK_SIZE / SECTOR_SIZE);
		sector_count = BLOCK_SIZE / SECTOR_SIZE;
		READ(sector_count, lba, io_buffer);

		//u16 
		READ_WORD(&io_buffer[inode_offset], inode->i_mode);
		//u16 
		READ_WORD(&io_buffer[inode_offset + 2], inode->i_uid);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 4], inode->i_size);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 8], inode->i_atime);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 12], inode->i_ctime);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 16], inode->i_mtime);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 20], inode->i_dtime);
		//u16
		READ_DWORD(&io_buffer[inode_offset + 24], inode->i_gid);
		//u16 
		READ_DWORD(&io_buffer[inode_offset + 26], inode->i_links_count);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 28], inode->i_blocks);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 32], inode->i_flags);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 36], inode->osd1);
		//u32[EXT2_N_BLOCKS]
		READ_DWORD(&io_buffer[inode_offset + 40], inode->i_block[0]);
		READ_DWORD(&io_buffer[inode_offset + 44], inode->i_block[1]);
		READ_DWORD(&io_buffer[inode_offset + 48], inode->i_block[2]);
		READ_DWORD(&io_buffer[inode_offset + 52], inode->i_block[3]);
		READ_DWORD(&io_buffer[inode_offset + 56], inode->i_block[4]);
		READ_DWORD(&io_buffer[inode_offset + 60], inode->i_block[5]);
		READ_DWORD(&io_buffer[inode_offset + 64], inode->i_block[6]);
		READ_DWORD(&io_buffer[inode_offset + 68], inode->i_block[7]);
		READ_DWORD(&io_buffer[inode_offset + 72], inode->i_block[8]);
		READ_DWORD(&io_buffer[inode_offset + 76], inode->i_block[9]);
		READ_DWORD(&io_buffer[inode_offset + 80], inode->i_block[10]);
		READ_DWORD(&io_buffer[inode_offset + 84], inode->i_block[11]);
		READ_DWORD(&io_buffer[inode_offset + 88], inode->i_block[12]);
		READ_DWORD(&io_buffer[inode_offset + 92], inode->i_block[13]);
		READ_DWORD(&io_buffer[inode_offset + 96], inode->i_block[14]);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 100], inode->i_generation);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 104], inode->i_file_acl);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 108], inode->i_dir_acl);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 112], inode->i_faddr);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 116], inode->osd2_1);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 120], inode->osd2_2);
		//u32 
		READ_DWORD(&io_buffer[inode_offset + 124], inode->osd2_3);
		aligned_kfree(io_buffer);
		inode->counter++;
		put_inode_cache(ext2->superblock->inode_cache, inode);
	}
	return inode;
}

void static write_inode(t_ext2* ext2, t_inode* inode, u8 store_on_disk)
{
	u32 group_number;
	u32 group_offset;
	u32 inode_table_offset;
	u32 inode_offset;
	t_group_block* group_block = NULL;
	u32 lba;
	u32 sector_count;
	char* io_buffer = NULL;
	u32 group_i_number;
	
    if (get_inode_cache(ext2->superblock->inode_cache, inode->i_number) == NULL)
	{
		put_inode_cache(ext2->superblock->inode_cache, inode);
	}
	else
	{
		inode->counter--;
	}
	if (store_on_disk == 1)
	{
		inode->status = 0;
		io_buffer = aligned_kmalloc(BLOCK_SIZE, 16);	
		group_number = (inode->i_number - 1) / ext2->superblock->s_inodes_per_group; 
		group_block = read_group_block(ext2, group_number);
		group_i_number = inode->i_number - (group_number * ext2->superblock->s_inodes_per_group) - 1;
		inode_table_offset = group_i_number * 128 / BLOCK_SIZE;
		inode_offset = (inode->i_number - 1) * 128 % BLOCK_SIZE;
		lba = ext2->partition_start_sector + (inode_table_offset + group_block->bg_inode_table) * (BLOCK_SIZE / SECTOR_SIZE);
		sector_count = BLOCK_SIZE / SECTOR_SIZE;
		READ(sector_count, lba, io_buffer);

		//u16
		WRITE_WORD(inode->i_mode, &io_buffer[inode_offset]) 
		//u16
		WRITE_WORD(inode->i_uid, &io_buffer[inode_offset + 2])
		//u32 
		WRITE_DWORD(inode->i_size, &io_buffer[inode_offset + 4])
		//u32 
		WRITE_DWORD(inode->i_atime, &io_buffer[inode_offset + 8])
		//u32 
		WRITE_DWORD(inode->i_ctime, &io_buffer[inode_offset + 12])
		//u32 
		WRITE_DWORD(inode->i_mtime, &io_buffer[inode_offset + 16])
		//u32 
		WRITE_DWORD(inode->i_dtime, &io_buffer[inode_offset + 20])
		//u16
		WRITE_WORD(inode->i_gid, &io_buffer[inode_offset + 24 ])
		//u16 
		WRITE_WORD(inode->i_links_count, &io_buffer[inode_offset + 26])
		//u32 
		WRITE_DWORD(inode->i_blocks, &io_buffer[inode_offset + 28])
		//u32 
		WRITE_DWORD(inode->i_flags, &io_buffer[inode_offset + 32])
		//u32 
		WRITE_WORD(inode->osd1, &io_buffer[inode_offset + 36])
		//u32[EXT2_N_BLOCKS]
		WRITE_DWORD(inode->i_block[0], &io_buffer[inode_offset + 40])
		WRITE_DWORD(inode->i_block[1], &io_buffer[inode_offset + 44])
		WRITE_DWORD(inode->i_block[2], &io_buffer[inode_offset + 48])
		WRITE_DWORD(inode->i_block[3], &io_buffer[inode_offset + 52])
		WRITE_DWORD(inode->i_block[4], &io_buffer[inode_offset + 56])
		WRITE_DWORD(inode->i_block[5], &io_buffer[inode_offset + 60])
		WRITE_DWORD(inode->i_block[6], &io_buffer[inode_offset + 64])
		WRITE_DWORD(inode->i_block[7], &io_buffer[inode_offset + 68])
		WRITE_DWORD(inode->i_block[8], &io_buffer[inode_offset + 72])
		WRITE_DWORD(inode->i_block[9], &io_buffer[inode_offset + 76])
		WRITE_DWORD(inode->i_block[10], &io_buffer[inode_offset + 80])
		WRITE_DWORD(inode->i_block[11], &io_buffer[inode_offset + 84])
		WRITE_DWORD(inode->i_block[12], &io_buffer[inode_offset + 88])
		WRITE_DWORD(inode->i_block[13], &io_buffer[inode_offset + 92])
		WRITE_DWORD(inode->i_block[14], &io_buffer[inode_offset + 96])
		//u32 
		WRITE_DWORD(inode->i_generation, &io_buffer[inode_offset + 100])
		//u32 
		WRITE_DWORD(inode->i_file_acl, &io_buffer[inode_offset + 104])
		//u32 
		WRITE_DWORD(inode->i_dir_acl, &io_buffer[inode_offset + 108])
		//u32 
		WRITE_DWORD(inode->i_faddr, &io_buffer[inode_offset + 112])
		//u32 
		WRITE_DWORD(inode->osd2_1, &io_buffer[inode_offset + 116])
		//u32 
		WRITE_DWORD(inode->osd2_2, &io_buffer[inode_offset + 120])
		//u32 
		WRITE_WORD(inode->osd2_3, &io_buffer[inode_offset + 124])
		WRITE(sector_count, lba, io_buffer);
		aligned_kfree(io_buffer);
	}
	else
	{
		inode->status = 1;
	}
}

void breakpoint()
{
	return;
}

static t_inode* read_dir_inode(char* file_name, t_inode* parent_dir_inode, t_ext2* ext2)
{
	int i;
	int j;
	u32 i_number;
	u32 next_entry;
	u32 name_len;
	u32 rec_len;
	char* io_buffer;
	u32 lba;
	u8 found_inode;
	char file_name_entry[NAME_MAX];
	//u32 ret_code = -1;
	u32 file_name_len;
	t_inode* inode = NULL;

	// For directory inode supposed max 12 block	
	for (i = 0; i <= 11; i++)
	{
		if (parent_dir_inode->i_block[i] == 0)
		{	
			break;
		} 
	}
	io_buffer = aligned_kmalloc(BLOCK_SIZE, 16);
	for (j = 0; j <= (i - 1); j++)
	{
		lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[j]);
		READ((BLOCK_SIZE / SECTOR_SIZE), lba, io_buffer);
		found_inode = 0;
		next_entry = 0;
		file_name_len = strlen(file_name);
		while(next_entry < BLOCK_SIZE)
		{
			READ_DWORD(&io_buffer[next_entry], i_number);
			READ_BYTE(&io_buffer[next_entry + 6], name_len);
			READ_WORD(&io_buffer[next_entry + 4], rec_len);
			if (i_number != 0 && strncmp(file_name, &io_buffer[next_entry + 8], file_name_len) == 0 && name_len == strlen(file_name)) 
			{
				found_inode = 1;
				goto EXIT;
			}
			next_entry += rec_len;
		}
	}

EXIT:
	if(found_inode == 1)
	{
		if (i_number == 0)
		{
			panic();
		}
		inode = read_inode(ext2, i_number);
	}
	else 
	{
		PRINTK("INODE NOT FOUND !!!!!!!!!!");
		inode = NULL;
	}
	aligned_kfree(io_buffer);
	return inode;
}

//Old version used for MBR partition
u32 static lookup_partition(t_ext2* ext2,u8 partition_number)
{
	char* io_buffer = NULL;
    u32 first_partition_sector;
	u32 partition_offset;
	u32 head;
	u32 sector;
	u32 cylinder;

	partition_number = 2;
	io_buffer = aligned_kmalloc(BLOCK_SIZE, 16);
	partition_offset = 446 + ((partition_number - 1) * 16) + 1;			
	P_READ(1, 0, io_buffer);
	head = io_buffer[partition_offset];
	sector = (io_buffer[partition_offset + 1]) & 0x3F;
	cylinder=((io_buffer[partition_offset + 1] & 0xc0) << 2) | io_buffer[partition_offset + 2];
    first_partition_sector = ((cylinder * 16) + head) * 63 + sector - 1;
    aligned_kfree(io_buffer);
    return first_partition_sector;
}

u32 static lookup_gpt_partition(t_ext2* ext2,u8 partition_number)
{
	unsigned char* io_buffer = NULL;
    u32 first_partition_sector;
	u32 partition_offset;
	
	io_buffer = aligned_kmalloc(BLOCK_SIZE, 16);		
	P_READ(1, 2, io_buffer);
	partition_offset = 32 + (partition_number - 1) * 128;
	first_partition_sector = io_buffer[partition_offset]
	                         + (io_buffer[partition_offset + 1] << 8) 
	                         + (io_buffer[partition_offset + 2] << 16)  
	                         + (io_buffer[partition_offset + 3] << 24);
	aligned_kfree(io_buffer);
	return first_partition_sector;
}

u32 static find_free_inode(u32 group_block_index, t_ext2* ext2, u32 condition)
{
	u32 inode_per_group;
	u32 avg_free_inode;
	u32 avg_free_block;
	u32 lba;
	u32 sector_count;
	int inode_number;
	u32 tot_group_block;
	t_group_block* group_block = NULL;
	
	char* io_buffer = NULL;
	char* current_byte = NULL;
	int i;
	int j;
	t_ext2 _ext2;

	i = 0;
	j = 0;
	inode_number = -1;
	group_block = read_group_block(ext2,group_block_index);  
    tot_group_block = TOT_FS_GROUP(ext2->superblock->s_blocks_count, ext2->superblock->s_blocks_per_group);
	avg_free_inode = ext2->superblock->s_free_inodes_count / tot_group_block;
	avg_free_block =  ext2->superblock->s_free_blocks_count / tot_group_block;
	inode_per_group = ext2->superblock->s_inodes_per_group;
        
	if (
	    (group_block->bg_free_inodes_count > avg_free_inode && 
	    	group_block->bg_free_blocks_count > avg_free_block && condition == 1)
	    ||
	    (group_block->bg_free_inodes_count > avg_free_inode && condition == 1)
	    || 
	    (condition) == 0)
   	{
		io_buffer = read_inode_bitmap(ext2, group_block->bg_inode_bitmap, group_block_index);
		current_byte = io_buffer;
        while (inode_number == -1 && i < (inode_per_group / 8))
       	{
        	while (inode_number == -1 && j < 8)
           	{
             	if (!((*current_byte) & (1 << j)))
               	{
                	inode_number = i * 8 + j + 1;//inode count starts from 1
                    *current_byte = *current_byte | 1 << j;
					write_inode_bitmap(ext2, group_block->bg_inode_bitmap, io_buffer, group_block_index, 0);
					group_block->bg_free_inodes_count--;
				    if (condition == 1)
					{
						group_block->bg_used_dirs_count++;
					}
					write_group_block(ext2,group_block_index,group_block, 0);
					ext2->superblock->s_free_inodes_count--;
                }
                j++;
          	}
            i++;
            j = 0;
			current_byte++;
    	}
	}
	if (inode_number != -1)
	{
		inode_number = inode_number + (group_block_index * ext2->superblock->s_inodes_per_group);
	}	
    return inode_number;
}

int static find_free_block(char* io_buffer, u32 group_block_index, u32 fs_tot_block,u32 fs_block_per_group, t_ext2* ext2)
{
   	u32 buffer_byte;
    u32 byte_bit;
    u32 selected_bit;
    u32 i;
	u32 buffer_byte2;
    u32 byte_bit2;
    u32 selected_bit2;
    u32 j;
	u32 free_block;
	int selected_block;
	u32 block_count;
	
	selected_block = -1;
	block_count = bitmap_block(fs_tot_block, fs_block_per_group, group_block_index);
    for (i = 0;i < block_count; i++)
    {
		//i is an index not a logical block number
		buffer_byte = RELATIVE_BITMAP_BYTE(i + 1, ext2->superblock->s_blocks_per_group);
		byte_bit = RELATIVE_BITMAP_BIT(i + 1, ext2->superblock->s_blocks_per_group);
        selected_bit = io_buffer[buffer_byte] & (1 << byte_bit);
        if (selected_bit == 0)
        {
			selected_block = i;
			break;
		}   
    }
	if (selected_block != -1)
	{
		io_buffer[buffer_byte] |= (1 << byte_bit);
		selected_block += (group_block_index * BLOCK_SIZE * 8);
		selected_block++;
	}
    return selected_block;      
}
