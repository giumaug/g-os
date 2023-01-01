struct s_inode;
struct s_ext2;
t_inode* lookup_inode(char* path, t_ext2* ext2);
static char* read_inode_bitmap(t_ext2* ext2, u32 bg_inode_bitmap, u32 group_block_index);
static void write_inode_bitmap(t_ext2* ext2, u32 bg_inode_bitmap, void* io_buffer, u32 group_block_index, u8 store_on_disk);
static char* read_block_bitmap(t_ext2* ext2, u32 bg_block_bitmap, u32 group_block_index);
static void write_block_bitmap(t_ext2* ext2, u32 bg_block_bitmap, void* io_buffer, u32 group_block_index, u8 store_on_disk);
static u32 read_indirect_block(t_ext2* ext2, t_inode* inode, u32 key);
static void free_indirect_block(t_ext2* ext2, t_inode* i_node);
static t_inode* read_dir_inode(char* file_name, t_inode* parent_dir_inode, t_ext2* ext2);
u32 static find_free_inode(u32 group_block_index, t_ext2 *ext2, u32 condition);
int static find_free_block(char* io_buffer, u32 group_block_pr_index, u32 fs_tot_block, u32 fs_block_per_group, t_ext2* ext2);
static t_inode* read_inode(t_ext2* ext2, u32 i_number);
u32 alloc_block(struct s_ext2* ext2, struct s_inode* i_node, u32 block_num);
static void write_group_block(t_ext2* ext2, u32 group_block_number, t_group_block* group_block, u8 store_on_disk);
void find_parent_path_and_filename(char* full_path, char* parent_path, char* filename);
static void indirect_block_free(t_indirect_block* indirect_block);
static t_indirect_block* indirect_block_init();
void static write_superblock(struct s_ext2* ext2);
void static write_inode(t_ext2* ext2, t_inode* inode, u8 store_on_disk);
static void fill_group_hash_1(t_ext2* ext2, t_llist* group_list, t_hashtable* group_hash, t_inode* i_node);
static void fill_group_hash_2(t_ext2* ext2, t_llist* group_list, t_hashtable* group_hash, u32 indirect_block, t_inode* i_node);
static void fill_group_hash_3(t_ext2* ext2, t_llist* group_list, t_hashtable* group_hash, t_inode* i_node);
t_group_block* read_group_block(t_ext2 *ext2,u32 group_block_number);
static void sync_fs(t_ext2* ext2);
static void put_inode_cache(struct s_inode_cache* inode_cache, struct s_inode* inode);
static struct s_inode* get_inode_cache(struct s_inode_cache* inode_cache, u32 i_number);
static void remove_inode_cache(struct s_inode_cache* inode_cache, u32 i_number);

extern unsigned int collect_mem;

int add_entry_to_dir(char* file_name, struct s_inode* parent_dir_inode, struct s_ext2* ext2, u32 inode_number, int file_type)
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
	u32 file_name_len;
	u32 new_rec_len;
	u8 found_entry;
	char* io_buffer = NULL;
	char* new_io_buffer = NULL;
	u32 new_entry;

	file_name_len = strlen(file_name);
	new_entry_len = 8 + file_name_len + (ENTRY_PAD((8 + file_name_len)));
	for (i = 0; i <= 12; i++)
	{
		if (parent_dir_inode->i_block[i] == 0)
		{	
			break;
		} 
	}
	if (i == 12 && parent_dir_inode->i_block[12] != 0)
	{
		return -1;
	}
	io_buffer = aligned_kmalloc(BLOCK_SIZE);
	lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[i - 1]);
	READ((BLOCK_SIZE / SECTOR_SIZE), lba, io_buffer);

	found_entry = 0;
	next_entry = 0;
	rec_len = 0;
	exit = 0;
	
	while (!exit)
	{
		READ_WORD(&io_buffer[next_entry + 4],rec_len);
		READ_BYTE(&io_buffer[next_entry + 6],cur_file_len);
		free_space = BLOCK_SIZE - next_entry - cur_file_len - 8 - (ENTRY_PAD((next_entry + 8 + cur_file_len)));
		if ((next_entry + rec_len) == BLOCK_SIZE && free_space >= new_entry_len)
		{
			found_entry = 1;
			new_entry = next_entry + 8 + cur_file_len + (ENTRY_PAD((next_entry + 8 + cur_file_len)));
			io_buffer[next_entry + 4] = (new_entry - next_entry) & 0xFF;
			io_buffer[next_entry + 5] = ((new_entry - next_entry) & 0xFF00) >> 8;
			new_rec_len = BLOCK_SIZE - new_entry;
			io_buffer[new_entry] = inode_number & 0xFF;                   	     //inode fourth word
			io_buffer[new_entry + 1] = (inode_number & 0xFF00) >> 8;             //inode third word
			io_buffer[new_entry + 2] = (inode_number & 0xFF0000) >> 16;          //inode second word
			io_buffer[new_entry + 3] = (inode_number & 0xFF000000) >> 24;        //inode first word
			io_buffer[new_entry + 4] = (new_rec_len & 0xFF);        	         //rec len second word
			io_buffer[new_entry + 5] = (new_rec_len & 0xFF00) >> 8;              //rec len first word
      		io_buffer[new_entry + 6] = file_name_len;                            //file len
			io_buffer[new_entry + 7] = file_type;                                //file type
			for (j = 0;j < file_name_len;j++)
			{
				io_buffer[new_entry + 8 + j] = file_name[j];
			}
			lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[i - 1]);
			WRITE((BLOCK_SIZE / SECTOR_SIZE), lba, io_buffer);
			if (file_type == DIRECTORY)
			{
				parent_dir_inode->i_links_count++;
				write_inode(ext2, parent_dir_inode, 1);
			}
			ret = 0;
			exit = 1;
		}
		else if ((next_entry + rec_len) == BLOCK_SIZE)
		{
			exit = 1;
		}
		next_entry += rec_len;
	}

	if (found_entry == 0 && i + 1 < 12)
	{
		new_rec_len = BLOCK_SIZE;
		//old_rec_len = (BLOCK_SIZE * i) - next_entry;
		parent_dir_inode->i_block[i] = alloc_block(ext2, parent_dir_inode, i);
		parent_dir_inode->status = 1;
		new_io_buffer = aligned_kmalloc(BLOCK_SIZE);
		kfillmem(new_io_buffer,0,BLOCK_SIZE);
		new_io_buffer[0] = inode_number & 0xFF;          	    //inode fourth word
		new_io_buffer[1] = (inode_number & 0xFF00) >> 8;     	//inode third word
		new_io_buffer[2] = (inode_number & 0xFF0000) >> 16;   	//inode second word
		new_io_buffer[3] = (inode_number & 0xFF000000) >> 24;  	//inode first word
		new_io_buffer[4] = (new_rec_len & 0xFF); 		        //rec len second word
		new_io_buffer[5] = (new_rec_len & 0xFF00) >> 8;   	    //rec len first word
		new_io_buffer[6] = file_name_len;                 	    //file len
		new_io_buffer[7] = file_type;                           //file type
		//io_buffer[next_entry + 4] = old_rec_len & 0xFF;
		//io_buffer[next_entry + 5] = (old_rec_len & 0xFF00) >> 8;
		for (j = 0;j < file_name_len;j++)
		{
			new_io_buffer[8 + j] = file_name[j];
		}
		lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[i - 1]);
		WRITE((BLOCK_SIZE / SECTOR_SIZE),lba, io_buffer);
		lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[i]);
		WRITE((BLOCK_SIZE / SECTOR_SIZE),lba,new_io_buffer);
		aligned_kfree(new_io_buffer);
		parent_dir_inode->i_size += BLOCK_SIZE;
        parent_dir_inode->i_blocks += 2;
		if (file_type == DIRECTORY)
		{
			parent_dir_inode->i_links_count++;
		}
		write_inode(ext2, parent_dir_inode, 1);
		ret = 0;
	}	
	aligned_kfree(io_buffer);
	return ret;
}

static void new_group_list(t_hashtable* group_hash, t_llist* group_list, u32 group_block_index, u32 block_addr)
{
	t_llist* block_list = NULL;
	u32* bucket = NULL;	

	block_list = new_dllist();
	bucket = kmalloc(sizeof(u32));
	*bucket = block_addr;
	ll_append(block_list, bucket);
	hashtable_put(group_hash, group_block_index, block_list);
	bucket = kmalloc(sizeof(u32));
	*bucket = group_block_index;
	ll_append(group_list, bucket);
}

static void update_group_list(t_llist* block_list, u32 block_addr)
{
	u32* bucket = NULL;

	bucket = kmalloc(sizeof(u32));
	*bucket = block_addr;
	ll_append(block_list, bucket);
}

static void fill_group_hash_1(t_ext2* ext2, t_llist* group_list, t_hashtable* group_hash, t_inode* i_node)
{
	u32 offset;
	u32 group_block_index;
	t_llist* block_list = NULL;

	offset = 0;
	while (i_node->i_block[offset] != NULL && offset <12)
	{
		group_block_index = (i_node->i_block[offset] - 1) / ext2->superblock->s_blocks_per_group;
		if ((block_list = hashtable_get(group_hash, group_block_index)) == NULL)
		{
			new_group_list(group_hash, group_list, group_block_index, i_node->i_block[offset]);	
		}
		else
		{
			update_group_list(block_list, i_node->i_block[offset]);
		}
		offset++;
	}
}

static void fill_group_hash_2(t_ext2* ext2, t_llist* group_list, t_hashtable* group_hash, u32 indirect_block, t_inode* i_node)
{
	u32 offset;
	u32 lba;
	u32 group_block_index;
	t_llist* block_list = NULL;
	u32 block_addr;
	char* io_buffer = NULL;

	io_buffer = aligned_kmalloc(BLOCK_SIZE);
	group_block_index = (indirect_block - 1) / ext2->superblock->s_blocks_per_group;
	lba = FROM_BLOCK_TO_LBA(indirect_block);
	READ(BLOCK_SIZE / SECTOR_SIZE, lba, io_buffer);

	if ((block_list = hashtable_get(group_hash, group_block_index)) == NULL)
	{
		new_group_list(group_hash, group_list, group_block_index, indirect_block);	
	}
	else
	{
		update_group_list(block_list, indirect_block);
	}
	offset = 0;
	READ_DWORD(&io_buffer[4 * offset], block_addr);
	while (block_addr != NULL && offset < 256)
	{
		group_block_index = (block_addr - 1) / ext2->superblock->s_blocks_per_group;
		if ((block_list = hashtable_get(group_hash, group_block_index)) == NULL)
		{
			new_group_list(group_hash, group_list, group_block_index, block_addr);	
		}
		else 
		{
			update_group_list(block_list, block_addr);	
		}
		READ_DWORD(&io_buffer[4 * ++offset], block_addr);
	}
	aligned_kfree(io_buffer);
}

static void fill_group_hash_3(t_ext2* ext2, t_llist* group_list, t_hashtable* group_hash, t_inode* i_node)
{
	u32 offset;
	u32 lba;
	u32 group_block_index;
	t_llist* block_list = NULL;
	u32 block_addr;
	char* io_buffer = NULL;

	io_buffer = aligned_kmalloc(BLOCK_SIZE);
	group_block_index = (i_node->i_block[13] - 1) / ext2->superblock->s_blocks_per_group;
	lba = FROM_BLOCK_TO_LBA(i_node->i_block[13]);
	READ(BLOCK_SIZE / SECTOR_SIZE, lba, io_buffer);

	if ((block_list = hashtable_get(group_hash, group_block_index)) == NULL)
	{
		new_group_list(group_hash, group_list, group_block_index, i_node->i_block[13]);	
	}
	else
	{
		update_group_list(block_list, i_node->i_block[13]);
	}
	offset = 0;
	READ_DWORD(&io_buffer[4 * offset], block_addr);
	while (block_addr != NULL && offset < 256)
	{
		fill_group_hash_2(ext2, group_list, group_hash, block_addr, i_node);
		READ_DWORD(&io_buffer[4 * ++offset], block_addr);
	}
	aligned_kfree(io_buffer);
}

void free_inode(t_inode* i_node, t_ext2* ext2)
{
	u32 fs_tot_group;
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
	t_llist_node* next_2 = NULL;
	t_llist_node* sentinel = NULL;
	t_llist_node* sentinel_2 = NULL;

	//1)find block descriptor conteinig inode	
	//2)clear inode_bitmap bit
	//3)clear_data_block_bitmap bit

	inode_prealloc_block_free(i_node, 0);
	//io_buffer = kmalloc(BLOCK_SIZE);
    group_block_index = (i_node->i_number - 1) / ext2->superblock->s_inodes_per_group;
	//group_block = kmalloc(sizeof(t_group_block));
	group_block = read_group_block(ext2, group_block_index);
	group_block->bg_free_inodes_count++;
	if (i_node->i_mode & 0x4000 )
	{
		group_block->bg_used_dirs_count--;
	}
	ext2->superblock->s_free_inodes_count++;
	write_group_block(ext2, group_block_index, group_block, 0);
	
	io_buffer = read_inode_bitmap(ext2, group_block->bg_inode_bitmap, group_block_index);
	inode_index = (i_node->i_number - 1) % ext2->superblock->s_inodes_per_group;
    buffer_byte = inode_index / 8;
    byte_bit = inode_index % 8;
    io_buffer[buffer_byte] &= ~(1 << byte_bit);
	write_inode_bitmap(ext2, group_block->bg_inode_bitmap, io_buffer, group_block_index, 0);
        
	fs_tot_group = TOT_FS_GROUP(ext2->superblock->s_blocks_count, ext2->superblock->s_blocks_per_group);
	//group_hash = dc_hashtable_init(fs_tot_group, &free_llist);
	group_hash = hashtable_init(fs_tot_group);
	group_list = new_dllist();

	fill_group_hash_1(ext2, group_list, group_hash, i_node);
	if (i_node->i_block[12] != NULL)
	{
		fill_group_hash_2(ext2, group_list, group_hash, i_node->i_block[12], i_node);
	}
	if (i_node->i_block[13] != NULL)
	{
		fill_group_hash_3(ext2, group_list, group_hash, i_node);
	}
	
	sentinel = ll_sentinel(group_list);
	next = ll_first(group_list);
	while (next != sentinel) 
	{
		group_block_key = *(u32*) next->val;
		block_list = hashtable_remove(group_hash, group_block_key);
		group_block = read_group_block(ext2, group_block_key);
		io_buffer = read_block_bitmap(ext2, group_block->bg_block_bitmap, group_block_key);		
		sentinel_2 = ll_sentinel(block_list);
		next_2 = ll_first(block_list);
		while (next_2 != sentinel_2) 
		{
			offset = *(u32*) next_2->val;
			buffer_byte = RELATIVE_BITMAP_BYTE(offset, ext2->superblock->s_blocks_per_group);
            byte_bit = RELATIVE_BITMAP_BIT(offset, ext2->superblock->s_blocks_per_group);
			io_buffer[buffer_byte] &= ~(1 << byte_bit);
			next_2 = ll_next(next_2);
			group_block->bg_free_blocks_count++;
			ext2->superblock->s_free_blocks_count++;
		}
		next = ll_next(next);
		write_block_bitmap(ext2, group_block->bg_block_bitmap, io_buffer,group_block_key, 0);
		write_group_block(ext2, group_block_key, group_block, 0);
		free_llist(block_list);
	}

	i_node->i_mode = 0;
	i_node->i_uid = 0; 
	i_node->i_size = 0;
    i_node->i_atime = 0;
	i_node->i_ctime = 0;
	i_node->i_mtime = 0;
	i_node->i_dtime = 0;
	i_node->i_gid = 0;
	i_node->i_links_count = 0;
	i_node->i_blocks = 0;
	i_node->i_flags = 0;
	write_inode(ext2, i_node, 1);
 	hashtable_free(group_hash);
	free_llist(group_list);
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
		byte_bit = RELATIVE_BITMAP_BIT(i, blocks_per_group);
		io_buffer[buffer_byte] &= ~(1 << byte_bit);
	}
	group_block->bg_free_blocks_count += i_node->preallocated_block_count;
	ext2->superblock->s_free_blocks_count += i_node->preallocated_block_count;
    i_node->first_preallocated_block = -1;
	i_node->preallocated_block_count = 0;
}

//inode count starts from 1
//group_index starts from 1
//files block count start from 0 (free to choice 0 or 1)
u32 alloc_block(t_ext2* ext2, t_inode* i_node, u32 block_num)
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

	u32 static fake_block = 0;
	fake_block++;
	
	block = -1;
        preferred_block = -1;
	group_block_index = -1;
        discard_preallocated_block = 1;
	from_preallocated = 0;
	//group_block = kmalloc(sizeof(t_group_block));
	//io_buffer = kmalloc(BLOCK_SIZE);

	fs_tot_block = ext2->superblock->s_blocks_count;
	fs_tot_group = TOT_FS_GROUP(ext2->superblock->s_blocks_count, ext2->superblock->s_blocks_per_group);
	blocks_per_group = ext2->superblock->s_blocks_per_group;
	group_block_index = (i_node->i_number - 1) / ext2->superblock->s_inodes_per_group;
    if (i_node->last_file_block_num != -1 && block_num == i_node->last_file_block_num + 1)
    {
		preferred_block = read_indirect_block(ext2, i_node, i_node->last_file_block_num) + 1;
    }
    else if (i_node->last_file_block_num != -1)
    {
		//File block number start from zero, logical block number from 1.
        offset = block_num - 1 + 1;
        while(preferred_block == -1 && offset != -1)
        {
			preferred_block = read_indirect_block(ext2, i_node, offset - 1) == 0 ? -1 : offset;
            offset--;
       	}
    }
	else 
	{
		group_block = read_group_block(ext2, group_block_index);
		first_block_data = FIRST_DATA_BLOCK(ext2, group_block);
		preferred_block = first_block_data;
	}
	group_block_pr_index = (preferred_block - 1)  / blocks_per_group;
	if (i_node->preallocated_block_count > 0)
	{
		group_block_alloc_index = (i_node->first_preallocated_block + i_node->preallocated_block_count -1) / blocks_per_group;
		if (group_block_pr_index != group_block_alloc_index)
		{
			group_block = read_group_block(ext2, group_block_alloc_index);
			io_buffer = read_block_bitmap(ext2, group_block->bg_block_bitmap, group_block_alloc_index);
			discard_prealloc_block(ext2, group_block, i_node, io_buffer);
		}
	}
	group_block = read_group_block(ext2, group_block_pr_index);
	io_buffer = read_block_bitmap(ext2, group_block->bg_block_bitmap, group_block_pr_index);

    if(preferred_block == i_node->first_preallocated_block && preferred_block != -1)
    {
		if (i_node->preallocated_block_count > 0)
        {
			block = preferred_block;
            discard_preallocated_block = 0;
			buffer_byte = RELATIVE_BITMAP_BYTE(block, blocks_per_group);
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
        byte_bit = RELATIVE_BITMAP_BIT(preferred_block, blocks_per_group);
        selected_bit = io_buffer[buffer_byte] & (1 << byte_bit);
        if (selected_bit == 0)
        {
            block = preferred_block;
			io_buffer[buffer_byte] |= (1 << byte_bit);
        }
        else
        {
			block_count = bitmap_block(fs_tot_block, blocks_per_group, group_block_index);
            for (i = 0;i < 16;i++) //check boundary
            {       
				if (((preferred_block + 1 + i) % (8 * BLOCK_SIZE)) < block_count)
				{
					buffer_byte = RELATIVE_BITMAP_BYTE((preferred_block + 1 + i), blocks_per_group); 
                    byte_bit = RELATIVE_BITMAP_BIT((preferred_block + 1 + i), blocks_per_group);
                    selected_bit = io_buffer[buffer_byte] & (1 << byte_bit);
                    if (selected_bit == 0)
                    {
                        block = preferred_block + 1 + i;
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
			block = find_free_block(io_buffer, group_block_pr_index, fs_tot_block, blocks_per_group, ext2);
        }
        if (block != -1)
        {
        	//nothing
        }
        else
        {
			for(i = 0;i < fs_tot_group;i++)
            {
				group_block = read_group_block(ext2, i);
				io_buffer = read_block_bitmap(ext2, group_block->bg_block_bitmap, i);
				block = find_free_block(io_buffer, i, fs_tot_block, blocks_per_group, ext2);
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
        write_block_bitmap(ext2, group_block->bg_block_bitmap, io_buffer, group_block_pr_index, 0);
		if (!from_preallocated)
		{
 			group_block->bg_free_blocks_count--;
			ext2->superblock->s_free_blocks_count--;
		}
		write_group_block(ext2, group_block_pr_index, group_block, 0);
		//write_superblock(ext2);
		i_node->last_file_block_num = block_num;
    }
	system.alloc_counter++;
	return block;
}

void _break()
{
	return;
}

t_inode* lookup_inode(char* path, t_ext2* ext2)
{
	u32 i_number;
	u32 found_inode;
    int i,j;
    t_inode* _parent_dir_inode = NULL;
    t_inode* tmp_inode = NULL;
	t_inode* inode = NULL;
	t_ext2*  _ext2 = NULL;
    char name[NAME_MAX];
	struct t_process_context* current_process_context = NULL;
	
	_ext2 = ext2;
	CURRENT_PROCESS_CONTEXT(current_process_context);
	found_inode = 0;
	if (path[0] == '/' && path[1] == '\0')
	{
		i_number = current_process_context->root_dir_inode_number;
		inode = read_inode(_ext2, i_number);
		return inode;
	}
	else if (path[0] == '.' && path[1] == '/' && path[2] == '\0')
	{
		i_number = current_process_context->current_dir_inode_number;
		inode = read_inode(_ext2, i_number);
		return inode;
	}
    if (path[0] == '/')
    {
		i_number = current_process_context->root_dir_inode_number;
		_parent_dir_inode = read_inode(_ext2, i_number);
		found_inode = 1;                              
                i = 1;
    }
    else if(path[0] == '.' && path[1] == '/')
  	{
		i_number = current_process_context->current_dir_inode_number;
		_parent_dir_inode = read_inode(_ext2, i_number);
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
				tmp_inode = read_dir_inode(name,_parent_dir_inode,_ext2);
				if (tmp_inode == NULL)
				{
					break;
				}
				_parent_dir_inode->counter--;
				_parent_dir_inode = tmp_inode;
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
			inode = read_dir_inode(name, _parent_dir_inode, _ext2);
			_parent_dir_inode->counter--;
		}
	}
	return inode;
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
	int i, index, offset;
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
    else if (index == 0)
	{
		parent_path[0] = '.';
		parent_path[1] = '/';
		parent_path[2] = '\0';
		find_file_name(full_path, file_name);
	}
	else
	{
		i = 0;
		offset = 0;
		while (i < (index + 1))
		{
    		if (full_path[offset] == '/')
    		{
        		i++;
    		}
    		parent_path[offset] = full_path[offset];
			offset++;
		}
		parent_path[offset - 1] = '\0';
		find_file_name((full_path + offset), file_name);
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
	inode_number = -1;
	tot_group_block = TOT_FS_GROUP(ext2->superblock->s_blocks_count, ext2->superblock->s_blocks_per_group);
        parent_dir_group_block_index = (inode_parent_dir->i_number - 1) / ext2->superblock->s_inodes_per_group;
        if (type == 0)
        {
                group_block_offset = 1;
                //inode_number = -1;
		//tot_group_block = ext2->superblock->s_blocks_count / ext2->superblock->s_blocks_per_group;
                //parent_dir_group_block_index = (inode_parent_dir->i_number - 1) / ext2->superblock->s_inodes_per_group;
                group_block_index = parent_dir_group_block_index;
                  
		inode_number = find_free_inode(group_block_index, ext2, 0);
		if (inode_number == -1)
		{
                	while ((group_block_index + group_block_offset)  < tot_group_block && inode_number == -1)
                	{                  
						inode_number = find_free_inode(group_block_index + group_block_offset,ext2, 0);
                       	group_block_offset <<= 1;
                	}
                	if (inode_number == -1)
                	{
                        	group_block_index = parent_dir_group_block_index + 2;
                        	while(inode_number == -1 && group_block_index<tot_group_block)
                        	{                
								inode_number = find_free_inode(group_block_index, ext2, 0);
								group_block_index++;
                        	}
                        	if (inode_number == -1)
                        	{
                                	group_block_index = 0;
                                	while(inode_number == -1 && group_block_index < parent_dir_group_block_index - 1)
                                	{                   
										inode_number = find_free_inode(group_block_index, ext2, 0);
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
				//parent_dir_group_block_index = (inode_parent_dir->i_number - 1) / ext2->superblock->s_inodes_per_group;
				group_block_index = 0;
                while(group_block_index < tot_group_block && inode_number == -1)
                {
			inode_number = find_free_inode(group_block_index, ext2, 1);
			group_block_index++;
                }
		if (inode_number == -1)
		{
			group_block_index = parent_dir_group_block_index;
			while(group_block_index < tot_group_block && inode_number == -1)
                	{
				inode_number = find_free_inode(group_block_index, ext2, 1);
				group_block_index++;
                	}
		}
		if (inode_number == -1)
		{
			group_block_index = 0;
			while(group_block_index < parent_dir_group_block_index && inode_number == -1)
			{
				inode_number = find_free_inode(group_block_index, ext2, 1);
				group_block_index++;
			}
		}	
        }
	//printk("allocated inode is %d \n",inode_number);
	return inode_number;
}

static int del_full_dir(t_ext2* ext2, t_inode* inode_dir, t_inode* inode_parent_dir)
{
	u32 i;
	u32 j;
	u32 offset;
	u32 current_rec_len;
	u32 lba;
	u32 i_number;
	char* iob_dir = NULL;
	t_inode* inode = NULL;
	u8 file_type;
	
	for (i = 0; i <= 12; i++)
	{
		if (inode_dir->i_block[i] == 0)
		{	
			break;
		} 
	}
	if (i == 12 && inode_dir->i_block[i] != 0)
	{
		return -1;
	}

	iob_dir = aligned_kmalloc(BLOCK_SIZE * i);
	for(j = 0; j < i; j++)
	{
		lba = FROM_BLOCK_TO_LBA(inode_dir->i_block[j]);
		READ((BLOCK_SIZE / SECTOR_SIZE), lba, iob_dir + (BLOCK_SIZE * j));
	}

	offset = 0;
	while (offset < (BLOCK_SIZE * i))
	{
		READ_DWORD(&iob_dir[offset], i_number);
		READ_BYTE(&iob_dir[offset + 7], file_type);
		if (i_number != inode_dir->i_number && i_number != inode_parent_dir->i_number)
		{
			inode = read_inode(ext2, i_number);
			if (file_type == DIRECTORY)
			{
				del_full_dir(ext2, inode, inode_dir);
			}
			inode->counter--;
			free_inode(inode, ext2);
		}
		//AGGIUNGERE RICORSIONE PER DIRECTORY ANNIDATE
		READ_WORD(&iob_dir[offset + 4], current_rec_len);
		offset += current_rec_len;
	}
	aligned_kfree(iob_dir);
	return 0;
}

static int del_dir_entry(t_ext2* ext2, t_inode* inode_dir, t_inode* inode)
{
	u32 i;
	u32 j;
	u32 lba;
	u32 current_inode_number;
	u32 block_offset;
	u32 found_entry;
	u32 previous_rec_offset;
	u32 previous_rec_len;
	u32 current_rec_len;
	char* iob_dir = NULL;
	int ret;
	
	ret = -1;
	found_entry = 0;
	for (i = 0; i <= 12; i++)
	{
		if (inode_dir->i_block[i] == 0)
		{	
			break;
		} 
	}
	if (i == 12 && inode_dir->i_block[i] != 0)
	{
		return -1;
	}
	iob_dir = aligned_kmalloc(BLOCK_SIZE);
	for(j = 0; j < i; j++)
	{
		lba = FROM_BLOCK_TO_LBA(inode_dir->i_block[j]);
		READ((BLOCK_SIZE / SECTOR_SIZE), lba, iob_dir);

		block_offset = 0;
		while (block_offset < BLOCK_SIZE)
		{
			READ_DWORD(&iob_dir[block_offset], current_inode_number);
			if (current_inode_number == inode->i_number)
			{
				found_entry = 1;
				READ_WORD(&iob_dir[block_offset + 4], current_rec_len);
				goto FOUND_ENTRY;
			}
			previous_rec_offset = block_offset;
			READ_WORD(&iob_dir[block_offset + 4], current_rec_len);
			block_offset += current_rec_len;
		}
	}

FOUND_ENTRY:
	if (found_entry)
	{
		if (block_offset == 0)
		{
			previous_rec_offset = 0;
			WRITE_DWORD(0 , &iob_dir[block_offset]);
		}
		else
		{
			READ_WORD(&iob_dir[previous_rec_offset + 4], previous_rec_len);
 			WRITE_WORD((previous_rec_len + current_rec_len), &iob_dir[previous_rec_offset + 4]);
		}
		lba = FROM_BLOCK_TO_LBA(inode_dir->i_block[j]);
		//READ_WORD(&iob_dir[previous_rec_offset + 4], previous_rec_len);
 		//WRITE_WORD((previous_rec_len + current_rec_len), &iob_dir[previous_rec_offset + 4]);
		WRITE(BLOCK_SIZE / SECTOR_SIZE, lba, iob_dir);
		free_inode(inode, ext2);
		remove_inode_cache(inode->ext2->superblock->inode_cache, inode->i_number);
		ret = 1;
	}
	aligned_kfree(iob_dir);
	return ret;
}

static int num_dir_entry(t_ext2* ext2, t_inode* inode_dir)
{
	u32 i;
	u32 j;
	u32 lba;
	u32 current_inode_number;
	u32 block_offset;
	u32 current_rec_len;
	char* iob_dir = NULL;
	int num_entry = 0;
	int ret;
   
	ret = -1;
	for (i = 0; i <= 12; i++)
	{
		if (inode_dir->i_block[i] == 0)
		{	
			break;
		} 
	}
	if (i == 12 && inode_dir->i_block[i] != 0)
	{
		return -1;
	}
	iob_dir = aligned_kmalloc(BLOCK_SIZE);
	for(j = 0; j < i; j++)
	{
		lba = FROM_BLOCK_TO_LBA(inode_dir->i_block[j]);
		READ((BLOCK_SIZE / SECTOR_SIZE), lba, iob_dir);

		block_offset = 0;
		while (block_offset < BLOCK_SIZE)
		{
			READ_DWORD(&iob_dir[block_offset], current_inode_number);
			READ_WORD(&iob_dir[block_offset + 4], current_rec_len);
			block_offset += current_rec_len;
			if (current_inode_number != 0 )
			{
				num_entry++;
			}
		}
	}
	aligned_kfree(iob_dir);
	return num_entry;
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
