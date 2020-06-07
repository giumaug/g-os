struct s_inode;
struct s_ext2;
u32 lookup_inode(char* path,t_ext2* ext2,t_inode* inode);
static void read_block_bitmap(t_ext2* ext2,u32 bg_block_bitmap,void* io_buffer);
static void write_block_bitmap(t_ext2* ext2,u32 bg_block_bitmap,void* io_buffer);
static u32 read_indirect_block(t_ext2* ext2,t_inode* inode,u32 key);
static void free_indirect_block(t_ext2* ext2,t_inode* i_node);
u32 static read_dir_inode(char* file_name,t_inode* parent_dir_inode,t_ext2* ext2,t_inode* inode);
u32 static find_free_inode(u32 group_block_index,t_ext2 *ext2,u32 condition);
int static find_free_block(char* io_buffer, u32 group_block_pr_index, u32 fs_tot_block,u32 fs_block_per_group);
void static read_inode(t_ext2* ext2,t_inode* inode);
u32 alloc_block(struct s_ext2* ext2,struct s_inode* i_node,u32 block_num);
static void write_group_block(t_ext2* ext2,u32 group_block_number,t_group_block* group_block);
void find_parent_path_and_filename(char* full_path,char* parent_path,char* filename);
static void indirect_block_free(t_indirect_block* indirect_block);
static t_indirect_block* indirect_block_init();
void static write_superblock(struct s_ext2* ext2);
void static write_inode(t_ext2* ext2,t_inode* inode);

int add_entry_to_dir(char* file_name,struct s_inode* parent_dir_inode,struct s_ext2* ext2,u32 inode_number)
{
	u32 free_space;
	u32 cur_file_len;
	u8 exit;
	u32 lba;
	int ret = -1;
	int i,j;
	u32 next_entry;
	u32 rec_len;
	u32 new_entry_len;
	u32 old_rec_len;
	u32 file_name_len;
	u32 new_rec_len;
	u32 block_index;
	u8 found_entry;
	char* io_buffer = NULL;
	char* new_io_buffer = NULL;
	u32 new_entry;

	file_name_len = strlen(file_name);
	new_entry_len = 8 + file_name_len + (ENTRY_PAD((8 + file_name_len)));
	for (i = 0;i <= 12;i++)
	{
		if (parent_dir_inode->i_block[i] == 0)
		{	
			break;
		} 
	}
	if (i == 12)
	{
		return -1;
	}
	io_buffer = kmalloc(BLOCK_SIZE * i);
	for(j = 0;j < i;j++)
	{
		lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[j]);
		READ((BLOCK_SIZE / SECTOR_SIZE),lba,io_buffer + (BLOCK_SIZE * j));
	}
	found_entry = 0;
	next_entry = 0;
	rec_len = 0;
	exit = 0;
	while(!exit)
	{
		READ_WORD(&io_buffer[next_entry + 4],rec_len);
		READ_BYTE(&io_buffer[next_entry + 6],cur_file_len);
		free_space = (BLOCK_SIZE * i) - next_entry - cur_file_len - 8 - (ENTRY_PAD((next_entry + 8 + cur_file_len)));
		//printk("next_entry is %d ",next_entry);
		//printk("rec_len is %d \n",rec_len);
		if ((next_entry + rec_len) == (BLOCK_SIZE * i) && free_space >= new_entry_len)
		{
			new_entry = next_entry + 8 + cur_file_len + (ENTRY_PAD((next_entry + 8 + cur_file_len)));
			io_buffer[next_entry + 4] = (new_entry - next_entry) & 0xFF;
			io_buffer[next_entry + 5] = ((new_entry - next_entry) & 0xFF00) >> 8;
			printk("new entry is %d \n",new_entry);	
                        //printk("old last entry size %d \n",(new_entry - next_entry));
			new_rec_len = (BLOCK_SIZE * i) - new_entry;
			io_buffer[new_entry] = inode_number & 0xFF;                   	     //inode fourth word
			io_buffer[new_entry + 1] = (inode_number & 0xFF00) >> 8;             //inode third word
			io_buffer[new_entry + 2] = (inode_number & 0xFF0000) >> 16;          //inode second word
			io_buffer[new_entry + 3] = (inode_number & 0xFF000000) >> 24;        //inode first word
			io_buffer[new_entry + 4] = (new_rec_len & 0xFF);        	     //rec len second word
			io_buffer[new_entry + 5] = (new_rec_len & 0xFF00) >> 8;              //rec len first word
                        io_buffer[new_entry + 6] = file_name_len;                            //file len
			io_buffer[new_entry + 7] = 1;                                        //file type
			for (j = 0;j < file_name_len;j++)
			{
				io_buffer[new_entry + 8 + j] = file_name[j];
			}
			block_index = next_entry / BLOCK_SIZE;
			lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[block_index]);
			WRITE((BLOCK_SIZE / SECTOR_SIZE),lba,(io_buffer + (block_index * BLOCK_SIZE)));
			ret = 1;
			exit = 1;
		}
		else if ((next_entry + rec_len) == (BLOCK_SIZE * i) && free_space < new_entry_len)
		{
			printk("qui!!!!!!!!!!!!!!!!! \n");
			if (i + 1 < 12)
			{
				new_rec_len = (BLOCK_SIZE * 1); // - new_entry_len;
				old_rec_len = (BLOCK_SIZE * 1) - next_entry;
				parent_dir_inode->i_block[i] = alloc_block(ext2,parent_dir_inode,(i + 1));
				new_io_buffer = kmalloc(BLOCK_SIZE);
				kfillmem(new_io_buffer,0,BLOCK_SIZE);
				new_io_buffer[0] = inode_number & 0xFF;          	//inode fourth word
				new_io_buffer[1] = (inode_number & 0xFF00) >> 8;     	//inode third word
				new_io_buffer[2] = (inode_number & 0xFF0000) >> 16;   	//inode second word
				new_io_buffer[3] = (inode_number & 0xFF000000) >> 24;  	//inode first word
				new_io_buffer[4] = (new_rec_len & 0xFF); 		//rec len second word
				new_io_buffer[5] = (new_rec_len & 0xFF00) >> 8;   	//rec len first word
				new_io_buffer[6] = file_name_len;                 	//file len
				new_io_buffer[7] = 1;                             	//file type
				io_buffer[next_entry + 4] = old_rec_len & 0xFF;
				io_buffer[next_entry + 5] = (old_rec_len & 0xFF00) >> 8;
				for (j = 0;j < file_name_len;j++)
				{
					new_io_buffer[8 + j] = file_name[j];
				}
				block_index = BLOCK_SIZE * (i - 1);
				lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[block_index]);
				WRITE((BLOCK_SIZE / SECTOR_SIZE),lba,(io_buffer + block_index));
				lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[i]);
				WRITE((BLOCK_SIZE / SECTOR_SIZE),lba,new_io_buffer);
				kfree(new_io_buffer);
				parent_dir_inode->i_size += BLOCK_SIZE;
				write_inode(ext2,parent_dir_inode);
				ret = 0;
			}
			exit = 1;
		}
		else 
		{
			next_entry += rec_len;
		}
	}	
	kfree(io_buffer);
	return ret;
}

static void fill_group_hash(t_ext2* ext2,t_llist* group_list,t_hashtable* group_hash,u32 start_block,u32 end_block,t_inode* i_node)
{
	u32 i;
	u32 lba;
	u32 group_block_index;
	t_llist* block_list = NULL;
	u32 block_addr;
	char* io_buffer = NULL;
	
	if (start_block >= 12 && end_block <= 1033)
	{
		io_buffer = kmalloc(BLOCK_SIZE);
		lba = FROM_BLOCK_TO_LBA(i_node->i_block[12]);
		READ(BLOCK_SIZE / SECTOR_SIZE, lba, io_buffer);
		block_list = new_dllist();
		ll_append(block_list, i_node->i_block[12]);
		hashtable_put(group_hash, group_block_index, block_list);
		ll_append(group_list, i_node->i_block[12]);
	}
	for (i = start_block; i <= end_block; i++)
	{
		if (i >= 0 && i <= 11)
		{
			block_addr = i_node->i_block[i];
		}
		else if (i >= 12 && i <= 1033)
		{
			READ_WORD(&io_buffer[i - 12], block_addr);
		}
		if (block_addr != 0)
		{
			group_block_index = block_addr / ext2->superblock->s_blocks_per_group; 
			if ((block_list = hashtable_get(group_hash, group_block_index)) == NULL)
			{
				block_list = new_dllist();
				ll_append(block_list, &i_node->i_block[i]);
				hashtable_put(group_hash, group_block_index, block_list);
				ll_append(group_list, block_addr);
			}
			else 
			{
				ll_append(block_list, block_addr);	
			}
		}
	}
	if (io_buffer != NULL)
	{
		kfree(io_buffer);
	}
}

void free_inode(t_inode* i_node, t_ext2* ext2)
{
        u32 group_block_index;
        u32 lba;
        u32 sector_count;
        u32 inode_index;
        u32 buffer_byte;
        u32 byte_bit;
	u32 group_block_key;
	u32 offset;
        char* io_buffer = NULL;
        t_group_block*  group_block = NULL;
	t_hashtable* group_hash = NULL;
	t_llist* block_list = NULL;
	t_llist* group_list = NULL;
	t_llist_node* next = NULL;
	t_llist_node* sentinel = NULL;
	t_llist_node* sentinel_2 = NULL;

	//1)find block descriptor conteinig inode	
	//2)clear inode_bitmap bit
	//3)clear_data_block_bitmap bit

        group_block_index = (i_node->i_number - 1) / ext2->superblock->s_inodes_per_group;
	group_block = kmalloc(sizeof(t_group_block));
	read_group_block(ext2, group_block_index, group_block);
	group_block->bg_free_inodes_count++;
	ext2->superblock->s_free_inodes_count++;
	write_group_block(ext2, group_block_index, group_block);
	
	read_block_bitmap(ext2, group_block->bg_inode_bitmap, io_buffer);
	inode_index = (i_node->i_number - 1) % ext2->superblock->s_blocks_per_group;
        buffer_byte = inode_index / 8;
        byte_bit = inode_index % 8;
        io_buffer[buffer_byte] &= ~(1 << byte_bit);
	write_block_bitmap(ext2, group_block->bg_inode_bitmap, io_buffer);
        
	group_hash = hashtable_init(50);//???????????????????????
	group_list = new_dllist();
	fill_group_hash(ext2, group_list, group_hash, 0, 11, i_node);
	if (i_node->i_block[12] != NULL)
	{
		fill_group_hash(ext2, group_list, group_hash, 12, 1033, i_node);
	}
	sentinel = ll_sentinel(group_list);
	next = ll_first(group_list);
	while (next != sentinel) 
	{
		group_block_key = next->val;
		block_list = hashtable_get(group_hash, group_block_key);
		read_group_block(ext2, group_block_key, group_block);
		read_block_bitmap(ext2, group_block->bg_block_bitmap, io_buffer);		
		sentinel_2 = ll_sentinel(block_list);
		next = ll_first(block_list);
		while (next != sentinel_2) 
		{
			offset = next->val;
			buffer_byte = offset / 8;
                	byte_bit = byte_bit=offset % 8;
			io_buffer[buffer_byte] &= ~(1 << byte_bit);
			next = ll_next(next);
			group_block->bg_free_blocks_count++;
			ext2->superblock->s_free_blocks_count++;
		}
		write_block_bitmap(ext2, group_block->bg_block_bitmap, io_buffer);
		write_group_block(ext2, group_block_key, group_block);
		free_llist(block_list);
	}
	if (i_node->i_block[12] != NULL)
	{
		free_indirect_block(ext2, i_node);
	}
	write_superblock(i_node->ext2);
 	hashtable_free(group_hash);
	free_llist(group_list);
	kfree(group_block);
	kfree(io_buffer);
}

static void prealloc_block(t_ext2* ext2, t_group_block* group_block, t_inode* i_node, char* io_buffer, u32 block)
{
	u32 i;
	u32 free_block;
	u32 rel_block;
	u32 buffer_byte;
	u32 byte_bit;
	u32 selected_bit;
	u32 first_preallocated_block;
	u32 fs_tot_block;
	u32 blocks_per_group;
	u32 block_count;
	u32 group_block_pr_index;

	free_block = 0;
	rel_block = block % (8 * BLOCK_SIZE);
	fs_tot_block = ext2->superblock->s_blocks_count;
	blocks_per_group = ext2->superblock->s_blocks_per_group;
	group_block_pr_index = block / blocks_per_group;
	block_count = bitmap_block(fs_tot_block, blocks_per_group, group_block_pr_index);

	if (rel_block + 9 < block_count)
	{
     		for(i = (rel_block + 1);i < (rel_block + 9);i++) //check boundary
            	{
			buffer_byte = RELATIVE_BITMAP_BYTE(i, blocks_per_group);
                   	byte_bit = RELATIVE_BITMAP_BIT(i, blocks_per_group);
			
			if (buffer_byte > 1023) 
			{
				panic();
			}

                      	selected_bit = io_buffer[buffer_byte] & (1 << byte_bit);
                      	if (selected_bit == 0)
          		{
                		free_block++;
                    	}
		}
  	}
   	if (free_block == PREALLOCATED_BLOCKS)
     	{
      		i_node->preallocated_block_count  = PREALLOCATED_BLOCKS;
		first_preallocated_block = block + 1;
          	i_node->first_preallocated_block = first_preallocated_block;
          	group_block->bg_free_blocks_count -= PREALLOCATED_BLOCKS;	
		ext2->superblock->s_free_blocks_count -= PREALLOCATED_BLOCKS;
		for(i = (rel_block + 1);i < (rel_block + PREALLOCATED_BLOCKS + 1);i++)
       		{
             		buffer_byte = RELATIVE_BITMAP_BYTE(i, blocks_per_group);
                      	byte_bit = RELATIVE_BITMAP_BIT(i, blocks_per_group);
                  	io_buffer[buffer_byte] =  io_buffer[buffer_byte] | (1 << byte_bit);   
             	}
	}
}

static void discard_prealloc_block(t_ext2* ext2, t_group_block* group_block, t_inode* i_node, char* io_buffer)
{
	int i;
	u32 buffer_byte;
	u32 byte_bit;
	u32 group_block_index;
	u32 blocks_per_group;

	blocks_per_group = ext2->superblock->s_blocks_per_group;
	for(i = i_node->first_preallocated_block; i < (i_node->first_preallocated_block + i_node->preallocated_block_count);i++)
	{
		buffer_byte = RELATIVE_BITMAP_BYTE(i, blocks_per_group);

		if (buffer_byte > 1023)
		{
			panic();
		}

            	byte_bit = RELATIVE_BITMAP_BIT(i, blocks_per_group);
		io_buffer[buffer_byte] &= ~(1 << byte_bit);
	}
	group_block->bg_free_blocks_count += i_node->preallocated_block_count;
	ext2->superblock->s_free_blocks_count += i_node->preallocated_block_count;
     	i_node->first_preallocated_block = -1;
}

//inode count starts from 1
//group_index starts from 0
//files block count start from 0
u32 alloc_block(t_ext2* ext2,t_inode* i_node,u32 block_num)
{
        char* io_buffer = NULL;
        s32 preferred_block;
        u32 lba;
        s32 block;
        s32 offset;
        t_group_block* group_block = NULL;
	s32 group_block_index;
	u32 block_bitmap;
        u32 sector_count;
        u32 free_block;
	u32 block_sector;
	u32 discard_preallocated_block;
	u8 is_preallocated_block_free;
	u32 buffer_byte;
	u32 byte_bit;
	u32 selected_bit;
	u32 inode_table_size;
	u32 first_block_data;
	u32 first_preallocated_block;
	u32 rel_block;
	u32 fs_tot_block;
	u32 fs_tot_group;
	u32 blocks_per_group;
	u32 group_block_pr_index;
	u32 group_block_alloc_index;
	u32 block_count;
	u8 from_preallocated;
        u32 i;
	u32 _tmp;
	
	block = -1;
        preferred_block = -1;
	group_block_index = -1;
        discard_preallocated_block = 1;
	from_preallocated = 0;
	group_block = kmalloc(sizeof(t_group_block));
	io_buffer = kmalloc(BLOCK_SIZE);

        sem_down(ext2->sem);
	fs_tot_block = ext2->superblock->s_blocks_count;
	fs_tot_group = TOT_FS_GROUP(ext2->superblock->s_blocks_count, ext2->superblock->s_blocks_per_group);
	blocks_per_group = ext2->superblock->s_blocks_per_group;
	group_block_index = (i_node->i_number - 1) / ext2->superblock->s_inodes_per_group;
        if (i_node->last_file_block_num != -1 && block_num == i_node->last_file_block_num + 1)
        {
		preferred_block = read_indirect_block(ext2,i_node,(i_node->last_file_block_num)) + 1;
        }
        else if (i_node->last_file_block_num != -1)
        {
                offset=block_num - 1;
                while(preferred_block == -1 && offset != -1)
                {
			preferred_block = read_indirect_block(ext2,i_node,offset) == 0 ? -1 : offset;
                        offset--;
                }
        }
	else 
	{
		read_group_block(ext2,group_block_index,group_block);
		first_block_data = FIRST_DATA_BLOCK(ext2,group_block);
		preferred_block = first_block_data;// + ABSOLUTE_BLOCK_ADDRESS(group_block_index,0);
	}
	group_block_pr_index = preferred_block / blocks_per_group;
	if (i_node->preallocated_block_count > 0)
	{
		group_block_alloc_index = (i_node->first_preallocated_block + i_node->preallocated_block_count) / blocks_per_group;
		if (group_block_pr_index != group_block_alloc_index)
		{
			read_group_block(ext2,group_block_alloc_index,group_block);
			read_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);
			discard_prealloc_block(ext2, group_block, i_node,io_buffer);
		}
	}
	read_group_block(ext2,group_block_pr_index,group_block);
	read_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);

        if(preferred_block == i_node->first_preallocated_block && preferred_block != -1)
        {
		if (i_node->preallocated_block_count > 0)
                {
			block = preferred_block;
                        discard_preallocated_block = 0;
			buffer_byte = RELATIVE_BITMAP_BYTE(block, blocks_per_group);

			if (buffer_byte > 1023)
			{
				panic();
			}

			byte_bit = RELATIVE_BITMAP_BIT(block, blocks_per_group);
			io_buffer[buffer_byte] |= (1 << byte_bit);
			i_node->first_preallocated_block++;
			i_node->preallocated_block_count--;
			from_preallocated = 1;
                }
		else
		{
			i_node->first_preallocated_block = -1;
		}
        }
        if (block == -1)
        {
		buffer_byte = RELATIVE_BITMAP_BYTE(preferred_block, blocks_per_group);

		if (buffer_byte > 1023)
		{
			panic();
		}		

                byte_bit = RELATIVE_BITMAP_BIT(preferred_block, blocks_per_group);
                selected_bit = io_buffer[buffer_byte] & (1 << byte_bit);
                if (selected_bit == 0)
                {
                        block = preferred_block;
			//buffer_byte = (preferred_block - first_block_data) / 8;
			//byte_bit = (preferred_block - first_block_data) % 8;
			io_buffer[buffer_byte] |= (1 << byte_bit);
                }
                else
                {
			block_count = bitmap_block(fs_tot_block, blocks_per_group, group_block_index);
                        for (i = 0;i < 16;i++) //check boundary
                        {       
				_tmp = ((preferred_block + 1 + i) % (8 * BLOCK_SIZE));
				if (((preferred_block + 1 + i) % (8 * BLOCK_SIZE)) < block_count)
				{
					buffer_byte = RELATIVE_BITMAP_BYTE((preferred_block + 1 + i), blocks_per_group); 
                                	byte_bit = RELATIVE_BITMAP_BIT((preferred_block + 1 + i), blocks_per_group);
                                	selected_bit = io_buffer[buffer_byte] & (1 << byte_bit);

					if (buffer_byte > 1023)
					{
						panic();
					}

                                	if (selected_bit == 0)
                                	{
                                        	block = preferred_block + 1 + i;
						//buffer_byte = RELATIVE_BITMAP_BYTE(block, blocks_per_group);
						//byte_bit = RELATIVE_BITMAP_BIT(block, blocks_per_group);
						io_buffer[buffer_byte] |= (1 << byte_bit);
                                        	break;  
                                	}
				}
				else
				{
					break;
				}
                        }
                }
                if (block == -1)
                {
                        //block = find_free_block(io_buffer,0);
			block = find_free_block(io_buffer, group_block_pr_index, fs_tot_block, blocks_per_group);
                }
                if (block != -1)
                {
                       //nothing
                }
                else
                {
			for(i = 0;i < fs_tot_group;i++)
                        {
				read_group_block(ext2,i,group_block);
				read_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);
				block = find_free_block(io_buffer, i, fs_tot_block, blocks_per_group);
                                if (block != -1)
                                {
					group_block_pr_index = i;
                                        break;
                                }
                        }
                }
        }
        if (block != -1)
        {
                if (discard_preallocated_block && i_node->first_preallocated_block != -1)
                {
			discard_prealloc_block(ext2, group_block, i_node, io_buffer);
             	}
		if (i_node->first_preallocated_block == -1)
		{
			prealloc_block(ext2, group_block, i_node, io_buffer, block);       
                }
		//buffer_byte = block / 8;
              	//byte_bit = block % 8;
                //io_buffer[buffer_byte] &= (255 & (2 >> byte_bit));
                write_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);
		if (!from_preallocated)
		{
 			group_block->bg_free_blocks_count--;
			ext2->superblock->s_free_blocks_count--;
		}
		write_group_block(ext2,group_block_pr_index,group_block);
		write_superblock(ext2);
		i_node->last_file_block_num = block_num;
        }
	kfree(group_block);
        kfree(io_buffer);
	sem_up(ext2->sem);
	//printk("allocating block %d \n",block);
	//printk("block num = %d \n",block_num);
	system.alloc_counter++;
	return block;
}

void _break()
{
	return;
}

u32 lookup_inode(char* path,t_ext2* ext2,t_inode* inode)
{
	u32 ret;
	u32 found_inode;
        int i,j;
        t_inode* _parent_dir_inode = NULL;
	t_inode* tmp_parent_dir_inode = NULL;
	t_ext2*  _ext2 = NULL;
        char name[NAME_MAX];
	struct t_process_context* current_process_context = NULL;
	
	_ext2 = ext2;
	CURRENT_PROCESS_CONTEXT(current_process_context);
	ret=-1;
	found_inode = 0;
	if (path[0] == '/' && path[1] == '\0')
	{
		inode->i_number = current_process_context->root_dir_inode_number;
		read_inode(_ext2,inode);
		return 0;
	}
	else if (path[0] == '.' && path[1] == '/' && path[2] == '\0')
	{
		inode->i_number = current_process_context->current_dir_inode_number;
		read_inode(_ext2,inode);
		return 0;
	}
	tmp_parent_dir_inode = inode_init(ext2);
        if (path[0] == '/')
        {
		tmp_parent_dir_inode->i_number = current_process_context->root_dir_inode_number;
		read_inode(_ext2,tmp_parent_dir_inode);
		found_inode = 1;                              
                i = 1;
        }
        else if(path[0] == '.' && path[1] == '/')
        {
//		CURRENT_PROCESS_CONTEXT(current_process_context);
		tmp_parent_dir_inode->i_number = current_process_context->current_dir_inode_number;
		read_inode(_ext2,tmp_parent_dir_inode);
		found_inode = 1;
                i = 2;    
        }
	else
	{
		PRINTK("\n");
		PRINTK("DEBUG:INODE NOT FOUND !!!!!!!!!!");
		PRINTK("\n");
	}
	if (found_inode)
	{	
		j = 0;
		_parent_dir_inode = tmp_parent_dir_inode;
		while (path[i] != '\0')
		{
			found_inode = 0;
			if (path[i] != '/') 
			{
				name[j++] = path[i++];
				found_inode = 1;
			}
			else 
			{
				name[j++] = '\0';
				j=0;
				ret = read_dir_inode(name,_parent_dir_inode,_ext2,inode);
				if (ret<0)
				{
					break;
				}
				_parent_dir_inode = inode;
				i++;
			}
		}
		name[j] = '\0';
		if (!found_inode)
		{
			PRINTK("\n");
			PRINTK("DEBUG:NOT FOUND INODE!!!!!!!!!!");
			PRINTK("\n");
		}
		else
		{
			ret = read_dir_inode(name,_parent_dir_inode,_ext2,inode);
		}
	}
	kfree(tmp_parent_dir_inode);
	return ret;
}

void find_file_name(char* full_path,char* file_name)
{
	u32 index;

	index = 0;
	while(full_path[index] != '\0')
	{
		file_name[index] =  full_path[index];	
		index++;
	}
	file_name[index] = '\0';
}

void find_parent_path_and_filename(char* full_path,char* parent_path,char* file_name)
{
	int i,index;
	char* path = NULL;

	path = full_path;
	if (path[0] == '/')
	{
		path++;
	}
	else if (path[0] == '.' && path[1] == '/')
	{
		path += 2;
	}
	index = 0;
	while(*path != '\0')
	{
		if(*path == '/')
		{
			index++;
		}
		path++;
	}
	if (index == 0 && full_path[0] == '/')
	{
		parent_path[0] = '/';
		parent_path[1] = '\0';
		find_file_name((full_path + 1),file_name);
	}
	else if (index == 0 && full_path[0] == '.' && full_path[1] == '/')
	{
		parent_path[0] = '.';
		parent_path[1] = '/';
		parent_path[2] = '\0';
		find_file_name((full_path + 2),file_name);
	}
	else
	{
		i = 0;
		while(i < index)
		{
			parent_path[i] = full_path[i];
			i++;
		}
		parent_path[i] = '\0';
		find_file_name((full_path + index),file_name);
	}
}

u32 alloc_inode(t_inode* inode_parent_dir ,unsigned int type,t_ext2 *ext2)
{
        int inode_number;      
        char* current_byte = NULL;
        u32 i,j;        
        u32 group_block_index;
        u32 parent_dir_group_block_index;
        u32 group_block_offset;
        void* io_buffer = NULL;
        u32 lba;
        u32 sector_count;
	u32 tot_group_block;

        // 1)seleziona inode parent dir
        // 2)seleziona  group descriptor inode  (block group = (inode – 1) / INODES_PER_GROUP)
        // 3)se bg_free_inodes_count>0  else step 4)
        //      3.1)leggi blocco inode bitmap (bg_inode_bitmap)
        //      3.2)seleziona primo inode libero
        //      3.3)update file system
        //      3.4)ritorna inode
        // 4)seleziona group descriptor group descriptor+1+2+4+.... inode mod(n=numero totale group descriptor)
        // 5)vai punto 3
        // 6)Seleziona primo group descriptor con inode libero a partire da group descriptor corrente +2
        if (type == 0)
        {
                group_block_offset = 1;
                inode_number = -1;
                tot_group_block = ext2->superblock->s_blocks_count / (1024 * (1 + ext2->superblock->s_log_block_size));
                parent_dir_group_block_index = (inode_parent_dir->i_number-1) / ext2->superblock->s_inodes_per_group;
                group_block_index = parent_dir_group_block_index;
                  
		inode_number = find_free_inode(group_block_index,ext2,0);
		if (inode_number == -1)
		{
                	while (group_block_index < tot_group_block && inode_number == -1)
                	{                  
				inode_number = find_free_inode(group_block_index + group_block_offset,ext2,0);
                        	group_block_offset >>= 1;
                	}
                	if (inode_number == -1)
                	{
                        	group_block_index = parent_dir_group_block_index + 2;
                        	while(inode_number != -1 && group_block_index<tot_group_block)
                        	{                
					inode_number = find_free_inode(group_block_index,ext2,0);
					group_block_index++;
                        	}
                        	if (inode_number == -1)
                        	{
                                	group_block_index = 0;
                                	while(inode_number != -1 && group_block_index < parent_dir_group_block_index - 1)
                                	{                   
						inode_number=find_free_inode(group_block_index,ext2,0);
                                	}
					group_block_index++;   
                        	}	
                	}
		}
        }
        //1)Seleziona primo group descriptor con numero inode<=media inode
        //      2.1)Leggi blocco inode bitmap (bg_inode_bitmap)
        //      2.2)Seleziona primo inode libero
        //      2.3)Update file system  
        //      2.4)ritorna inode
        //2)Seleziona primo group descriptor con inode libero a partire da group descriptor corrente +1
        //3)Vai punto 2.1
        else if (type == 1)
        {
		parent_dir_group_block_index = (inode_parent_dir->i_number-1) / ext2->superblock->s_inodes_per_group;
		group_block_index = 0;
                while(group_block_index < tot_group_block && inode_number == -1)
                {
			inode_number = find_free_inode(group_block_index,ext2,2);
			group_block_index++;
                }
		if (inode_number == -1)
		{
			group_block_index = parent_dir_group_block_index;
			while(group_block_index < tot_group_block && inode_number == -1)
                	{
				inode_number = find_free_inode(group_block_index,ext2,2);
				group_block_index++;
                	}
		}
		if (inode_number == -1)
		{
			group_block_index = 0;
			while(group_block_index < parent_dir_group_block_index && inode_number == -1)
			{
				inode_number = find_free_inode(group_block_index,ext2,2);
				group_block_index++;
			}
		}	
        }
	return inode_number;
}

/*
static int add_dir_entry(struct s_ext2* ext2,struct s_inode* inode_dir,u32 inode_number,char* filename,u32 type)
{
	u32 offset;
	u32 found_entry;
	char* iob_dir;
	int ret_val;
	u32 rec_len;
	u32 new_entry;
	u32 file_len;
	u32 file_type;
	
	ret_val=-1;
	iob_dir=kmalloc(BLOCK_SIZE);
	offset=0;
	while (inode_dir->i_block[offset]!=0)
	{
		offset++;
	}
	READ(BLOCK_SIZE/SECTOR_SIZE,inode_dir->i_block[offset],iob_dir);
	while(found_entry || offset==(BLOCK_SIZE-1))
	{
		inode_number=iob_dir[offset];
		rec_len=iob_dir[offset+4];
		if (inode_number==0)
		{
			found_entry=1;
		}
		else
		{
			offset=rec_len;
		}
	}
	if (found_entry)
	{
		new_entry=offset+8+file_len;
		if ((new_entry % 4)!=0)
		{
			new_entry=new_entry+(4-(new_entry % 4));
		}
		iob_dir[offset]=inode_number;
		iob_dir[offset+4]=new_entry;
		iob_dir[offset+6]=file_len;
		iob_dir[offset+7]=file_type;
		kmemcpy(iob_dir+offset,filename,file_len);
		ret_val=0;
	}
	kfree(iob_dir);
	return ret_val;
}
*/

static int del_full_dir(t_ext2* ext2, t_inode* inode_dir)
{
	u32 i;
	u32 offset;
	u32 current_rec_len;
	char* iob_dir = NULL;
	t_inode* inode = NULL;
	
	iob_dir = kmalloc(BLOCK_SIZE);
	inode = kmalloc(sizeof(t_inode));
	i = 0;
	offset = 0;
	sem_down(ext2->sem);
	while (inode_dir->i_block[i] != NULL && i < 12)
	{
		READ(BLOCK_SIZE/SECTOR_SIZE, inode_dir->i_block[i], iob_dir);
		while (offset < BLOCK_SIZE)
		{
			READ_DWORD(&iob_dir[offset], inode->i_number);
			read_inode(ext2, inode);
			free_inode(inode, ext2);
			READ_WORD(&iob_dir[offset + 4], current_rec_len);
			offset += current_rec_len;
		}
		i++;
	}
	sem_up(ext2->sem);
	kfree(iob_dir);
	kfree(inode);
	return 0;
}

static int del_dir_entry(t_ext2* ext2, t_inode* inode_dir, t_inode* inode)
{
	u32 offset;
	u32 current_inode_number;
	u32 block_offset;
	u32 found_entry;
	u32 previous_rec_offset;
	u32 previous_rec_len;
	u32 current_rec_len;
	char* iob_dir = NULL;
	int ret;
	
	ret = -1;
	iob_dir = kmalloc(BLOCK_SIZE);
	offset = 0;
	sem_down(ext2->sem);
	while (inode_dir->i_block[offset] < 12 && !found_entry)
	{
		READ(BLOCK_SIZE/SECTOR_SIZE, inode_dir->i_block[offset], iob_dir);
		for (block_offset = 0; block_offset < BLOCK_SIZE; block_offset++)
		{
			READ_DWORD(&iob_dir[offset], current_inode_number);
			if (current_inode_number == inode->i_number)
			{
				found_entry = 1;
				goto EXIT;
			}
		}
		previous_rec_offset = offset;
		READ_WORD(&iob_dir[offset + 4], current_rec_len);
		offset += current_rec_len;
	}
EXIT:
	if (found_entry)
	{
		READ_WORD(&iob_dir[previous_rec_offset + 4], previous_rec_len);
		WRITE_DWORD(0, &iob_dir[offset]);
 		WRITE_WORD((previous_rec_len + current_rec_len), &iob_dir[previous_rec_offset + 4]);
		WRITE(BLOCK_SIZE / SECTOR_SIZE, inode_dir->i_block[offset], iob_dir);
		free_inode(inode, ext2);
		ret = 1;
	}
	sem_up(ext2->sem);
	return ret;
}

static void extract_filename(char* fullpath,char* path,char* filename)
{
	int i;
	int j;
	int last_element;
	
	i=0;
	j=0;
	last_element=0;
	while(fullpath[i]!='\0')
	{
		if (fullpath[i]=='/')
		{
			last_element=i;
		}
		i++;	
	}
	for (j=0;j<last_element;j++)
	{
		path[j]=fullpath[j];
	}
	path[j]='\0';
	for (j=0;j<(i-last_element-1);j++)
	{
		filename[j]=fullpath[last_element+1+j];
	}
	filename[j]='\0';
}

void update_inode(t_inode* inode)
{
	u16 i_mode;
	u16 i_uid; 
	u32 i_size;
	u32 i_atime;
	u32 i_ctime;
	u32 i_mtime;
	u32 i_dtime;
	u16 i_gid;
	u16 i_links_count;
	u32 i_blocks;
	u32 i_flags;

}
