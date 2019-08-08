static void* read_block_bitmap(t_ext2* ext2,u32 bg_block_bitmap,void* io_buffer);
static void* write_block_bitmap(t_ext2* ext2,u32 bg_block_bitmap,void* io_buffer);
static u32 read_indirect_block(t_inode* inode,u32 key);
static void write_indirect_block(t_inode* inode,u32 key,u32 value);
static void free_indirect_block(t_ext2* ext2,t_inode* i_node);
u32 static read_dir_inode(char* file_name,t_inode* parent_dir_inode,t_ext2* ext2,t_inode* inode);
u32 static find_free_inode(u32 group_block_index,t_ext2 *ext2,u32 check_threshold);
u32 static find_free_block(char* io_buffer,u32 prealloc);
void static read_inode(t_ext2* ext2,t_inode* inode);
u32 lookup_inode(char* path,t_ext2* ext2,t_inode* inode);

int add_entry_to_dir(char* file_name,i_node parent_dir_inode,t_ext2* ext2,u32 inode_number)
{
	int ret = -1;
	int i,j;
	u32 new_entry_len;
	u32 old_rec_len;
	u32 file_name_len;
	u32 new_rec_len;
	u32 block_index;
	u8 pad;
	u8 found_entry;
	char* io_buffer = NULL;
	char* new_io_buffer = NULL;

	new_entry_len = 8 + file_name_len;
	pad = new_entry_len % 4;
	io_buffer = kmalloc(BLOCK_SIZE);
	file_name_len = strlen(file_name);
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
	for(j = 0;j < BLOCK_SIZE * i;j++)
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
		READ_WORD(&io_buffer[next_entry + 6],cur_file_len);
		free_space = (BLOCK_SIZE * i) - next_entry - cur_file_len;
		if (next_entry + rec_len = (BLOCK_SIZE * i) && free_space >= new_entry_len)
		{			
			new_rec_len = (BLOCK_SIZE * i) - next_entry - new_entry_len;
			io_buffer[next_entry] = inode_number && 0xFF;                   //inode fourth word
			io_buffer[next_entry + 1] = inode_number && 0x00FF;             //inode third word
			io_buffer[next_entry + 2] = inode_number && 0x0000FF;           //inode second word
			io_buffer[next_entry + 3] = inode_number && 0x000000FF;         //inode first word
			io_buffer[next_entry + 4] = (new_rec_len + pad) && 0xFF;        //rec len second word
			io_buffer[next_entry + 5] = (new_rec_len + pad) && 0x00FF;      //rec len first word
                        io_buffer[next_entry + 6] = file_name_len;                      //file len
			io_buffer[next_entry + 7] = 1;                                  //file type
			for (j = 0;j < file_name_len;j++)
			{
				io_buffer[next_entry + 8 + j] = file_name[j];
			}
			block_index = next_entry / BLOCK_SIZE;
			lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[block_index]);
			//WRITE((BLOCK_SIZE / SECTOR_SIZE),lba,(io_buffer + block_index));  !!!!!!!!!!!!!only for test
			ret = 1;
			exit = 1;
		}
		else if (next_entry + rec_len = (BLOCK_SIZE * i) && free_space < new_entry_len)
		{
			if (i + 1 < 12)
			{
				new_rec_len = (BLOCK_SIZE * (i + 1)) - new_entry_len;
				old_rec_len = (BLOCK_SIZE * (i + 1));
				parent_dir_inode->i_block[i + 1] = alloc_block(ext2,parent_dir_inode,(i + 1));
				new_io_buffer = kmalloc(BLOCK_SIZE);
				kfillmem(new_io_buffer,0,BLOCK_SIZE);
				new_io_buffer[0] = inode_number && 0xFF;          //inode fourth word
				new_io_buffer[1] = inode_number && 0x00FF;  	  //inode third word
				new_io_buffer[2] = inode_number && 0x0000FF;      //inode second word
				new_io_buffer[3] = inode_number && 0x000000FF;    //inode first word
				new_io_buffer[4] = (new_rec_len + pad) && 0x00FF; //rec len second word
				new_io_buffer[5] = (new_rec_len + pad) && 0xFF;   //rec len first word
				new_io_buffer[6] = file_name_len;                 //file len
				new_io_buffer[7] = 1;                             //file type
				io_buffer[next_entry + 4] = old_rec_len && 0xFF;
				io_buffer[next_entry + 5] = old_rec_len && 0x00FF;
				block_index = BLOCK_SIZE * i;
				lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[block_index]);
				//WRITE((BLOCK_SIZE / SECTOR_SIZE),lba,(io_buffer + block_index));  !!!!!!!!!!!!!only for test
				lba = FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[i + 1]);
				//WRITE((BLOCK_SIZE / SECTOR_SIZE),lba,new_io_buffer);              !!!!!!!!!!!!!only for test
				kfree(new_io_buffer);
				//write_inode(ext2,parent_dir_inode);    			    !!!!!!!!!!!!!only for test
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

//BROKEN!!!!!!!!!!!!!!!
static void fill_group_hash(t_ext2* ext2,t_llist* group_list,t_hashtable* group_hash,u32 start_block,u32 end_block,t_inode* i_node)
{
	u32 i;
	u32 group_block_index;
	t_llist* block_list;
	u32 block_index;
	u32* inode_block;
	
	if (start_block>=0 && end_block<=11)
	{
		inode_block=i_node->i_block;
	}
	else if (start_block>=12 && end_block<=1033)
	{
		//NOT WORKING AFTER NEW INDIRECT BLOCK LOGIC!!!!
		//inode_block=i_node->indirect_block;
	}

	for (i=start_block;i<=end_block;i++)
	{
		if (block_index=inode_block[i]!=0)
		{
			group_block_index=(block_index-1)/ext2->superblock->s_blocks_per_group; 
			if ((block_list=hashtable_get(group_hash,group_block_index))==NULL)
			{
				block_list=new_dllist();
				ll_append(block_list,&i_node->i_block[i]);
				hashtable_put(group_hash,group_block_index,block_list);
				ll_append(group_list,block_index);
			}
			else 
			{
				ll_append(block_list,block_index=&i_node->i_block[i]);	
			}
		}
	}
}

void free_inode(t_inode* i_node,t_ext2 *ext2)
{
        u32 group_block_index;
        u32 lba;
        u32 sector_count;
        u32 inode_index;
        u32 buffer_byte;
        u32 byte_bit;
	u32 group_block_key;
	u32 offset;
        char* io_buffer;
	char* io_buffer2;
        t_group_block*  group_block;
	t_hashtable* group_hash;
	t_llist* block_list;
	t_llist* group_list;
	t_llist_node* next;
	t_llist_node* sentinel;
	t_llist_node* sentinel2;
	t_device_desc* device_desc;

	//1)find block descriptor conteinig inode	
	//2)clear inode_bitmap bit
	//3)clear_data_block_bitmap bit

        group_block_index=(i_node->i_number-1)/ext2->superblock->s_inodes_per_group;
	group_block=kmalloc(sizeof(t_group_block));
	read_group_block(ext2,group_block_index,group_block);
	group_block->bg_free_inodes_count++;
	write_group_block(ext2,group_block_index,group_block);
	
       
        lba=ext2->partition_start_sector+(group_block->bg_inode_bitmap*BLOCK_SIZE/SECTOR_SIZE);
        sector_count=BLOCK_SIZE/SECTOR_SIZE;
        io_buffer=kmalloc(BLOCK_SIZE);
       	READ(sector_count,lba,io_buffer); 
	
	inode_index=(i_node->i_number - 1)%ext2->superblock->s_blocks_per_group;
        buffer_byte=(inode_index-1) / 8;
        byte_bit=(inode_index-1) % 8;
        io_buffer[buffer_byte]&= (255 & (2>>byte_bit));

	WRITE(sector_count,lba,io_buffer);
        
	lba=ext2->partition_start_sector+i_node->i_block[12]*(BLOCK_SIZE/SECTOR_SIZE)-1;
	sector_count=BLOCK_SIZE/SECTOR_SIZE;
       	READ(sector_count,lba,io_buffer);

	group_hash=hashtable_init(50);
	group_list=new_dllist();
	fill_group_hash(ext2,group_list,group_hash,0,11,i_node);
	if (i_node->i_block[12]!=NULL)
	{
		fill_group_hash(ext2,group_list,group_hash,12,1033,i_node);
	}

	sentinel=ll_sentinel(group_list);
	next=ll_first(group_list);
	while (next!=sentinel) 
	{
		group_block_key=next->val;
		block_list=hashtable_get(group_hash,group_block_key);

		read_group_block(ext2,group_block_key,group_block);
		read_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);
		
		sentinel2=ll_sentinel(block_list);
		next=ll_first(block_list);
		while (next!=sentinel2) 
		{
			offset=next->val;
			buffer_byte=(offset-1) / 8;
                	byte_bit=byte_bit=(offset-1) % 8;
			io_buffer[buffer_byte]&= (255 & (2>>byte_bit));
			next=ll_next(next);
			group_block->bg_free_blocks_count++;
			ext2->superblock->s_free_blocks_count++;
		}
		write_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);
		write_group_block(ext2,group_block_key,group_block);
		free_llist(block_list);
	}
	if (i_node->i_block[12]!=NULL)
	{
		free_indirect_block(ext2,i_node);
	}
	hashtable_free(group_hash);
	free_llist(group_list);
	ext2->superblock->s_free_inodes_count++;
	kfree(group_block);
	kfree(io_buffer);
}

//inode count starts from 1
//group_index starts from 0
//files block count start from 0
u32 alloc_block(t_ext2* ext2,t_inode* i_node,u32 block_num)
{
        char* io_buffer = NULL;
	char* indirect_block = NULL;   
        u32 preferred_block;
        u32 lba;
        u32 block;
        u32 offset;
        t_group_block* group_block;
	u32 group_block_index;
	u32 block_bitmap;
        u32 sector_count;
        u32 free_block;
	u32 block_sector;
	u32 discard_preallocated_block;
	u32 first_preallocated_block;
	u32 buffer_byte;
	u32 byte_bit;
	u32 selected_bit;
        u32 i;
	
	block = 0;
        preferred_block = 0;
        discard_preallocated_block = 1;
	group_block_index = (i_node->i_number - 1) / ext2->superblock->s_inodes_per_group;
	group_block = kmalloc(sizeof(t_group_block));
	read_group_block(ext2,group_block_index,group_block);
	
        io_buffer = kmalloc(BLOCK_SIZE);
	read_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);

        if (block_num = i_node->last_file_block_num + 1)
        {
		indirect_block = kmalloc(BLOCK_SIZE);--------------qui!!!!!!!!
		preferred_block = read_indirect_block(i_node,(i_node->last_file_block_num + 1),indirect_block); 
        }
        else if (i_node->last_file_block_num !=0)
        {
                offset=block_num - 1;
                while(preferred_block == 0 && offset != 0)
                {
			indirect_block = kmalloc(BLOCK_SIZE);
			preferred_block = read_indirect_block(i_node,offset,indirect_block) == 0 ? 0 : offset;
                        offset--;
                }
                if (preferred_block == 0)
                {
			preferred_block = ABSOLUTE_BLOCK_ADDRESS(group_block_index,0);
                }
        }
        if(preferred_block >= i_node->first_preallocated_block && preferred_block < i_node->preallocated_block_count)
        {
                block = preferred_block;
                if (--i_node->preallocated_block_count > 0)
                {
                        first_preallocated_block = ++i_node->first_preallocated_block;
                        discard_preallocated_block = 0;
                }
        }
        if (block == 0)
        {
		buffer_byte= preferred_block / 8;
                byte_bit = preferred_block  % 8;
                selected_bit = io_buffer[buffer_byte] & (2 >> byte_bit);
                if (selected_bit == 0)
                {
                        block = preferred_block;
                }
                else
                {
                        for (i = 0;i < 16;i++)
                        {
                                buffer_byte = (preferred_block + 1 + i) / 8;
                                byte_bit = (preferred_block + 1 + i) % 8;
                                selected_bit = io_buffer[buffer_byte] & (2 >> byte_bit);
                                if (selected_bit == 0)
                                {
                                        block = preferred_block + 1 + i;
                                        break;  
                                }
                        }
                }
                if (block == 0)
                {
                        block = find_free_block(io_buffer,0);
                }
                if (block != 0)
                {
                       //nothing
                }
                else
                {
			for(i = 0;i < ext2->superblock->s_blocks_count;i++)
                        {
				read_group_block(ext2,i,group_block);
				read_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);
                                block = find_free_block(io_buffer,0);
                                if (block != 0)
                                {
					group_block_index = i;
                                        break;
                                }
                        }
                }
        }
        if (block != 0)
        {
                if (discard_preallocated_block)
                {
			for(i = i_node->first_preallocated_block; i < (i_node->first_preallocated_block + i_node->preallocated_block_count);i++)
			{
				buffer_byte = i / 8;
                                byte_bit = i  % 8;
				io_buffer[buffer_byte] &= ~(2>>byte_bit);
			}
			group_block->bg_free_blocks_count += i_node->preallocated_block_count;
			ext2->superblock->s_free_blocks_count += i_node->preallocated_block_count;
			i_node->preallocated_block_count = 0;
                      	i_node->first_preallocated_block = 0;
                        free_block = 0;                  
                        for(i = (block + 1);i < (block + 9);i++)
                        {
                                buffer_byte = i / 8;
                                byte_bit = i % 8;
                                selected_bit = io_buffer[buffer_byte] & (2 >> byte_bit);
                                if (selected_bit == 0)
                                {
                                        free_block++;
                                }
                        }
                        if (free_block == 8)
                        {
                                i_node->preallocated_block_count = 8;
                                i_node->first_preallocated_block = block + 1;     
                                group_block->bg_free_blocks_count -= 8;
				ext2->superblock->s_free_blocks_count -= 8;
				for(i = (block + 1);i < (block + 9);i++)
                        	{
                                	buffer_byte = i / 8;
                                	byte_bit = i % 8;
                                	io_buffer[buffer_byte] |=  (2 >> byte_bit);     
                        	}                      
                }
		buffer_byte = block / 8;
              	byte_bit = block % 8;
                io_buffer[buffer_byte] &= (255 & (2 >> byte_bit));
                //write_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer); !!!!!!!!!!!!!only for test
 		group_block->bg_free_blocks_count--;
		//write_group_block(ext2,group_block_index,group_block); 	   !!!!!!!!!!!!!only for test
		ext2->superblock->s_free_blocks_count--;
		//write_superblock(ext2);					   !!!!!!!!!!!!!only for test
		i_node->last_file_block_num = block_num;
        }
	kfree(group_block);
        kfree(io_buffer);
	if (indirect_block != NULL)
	{
		kfree(indirect_block);
	}
	return BLOCK_SECTOR_ADDRESS(group_block_index,block);
}

void free_block()
{
	//nothing
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
	tmp_parent_dir_inode = inode_init();
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

void find_child_path(char* full_path,char* filename)
{
	u32 index;

	index = 0;
	while(full_path[index] != '\0')
	{
		filename[index] =  full_path[index];	
		index++;
	}
}

void find_parent_and_child_path(char* full_path,char* parent_path,char* filename)
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
		find_child_path((full_path + 1),filename);
	}
	else if (index == 0 && full_path[0] == '.' && full_path[1] == '/')
	{
		parent_path[0] = '.';
		parent_path[1] = '/';
		parent_path[2] = '\0';
		find_child_path((full_path + 2),filename);
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
		find_child_path((full_path + index),filename);
	}
}

void alloc_inode(t_inode* inode_parent_dir ,unsigned int type,t_ext2 *ext2)
{
        int inode_number;      
        char* current_byte = NULL;
        u32 i,j;        
        u32 group_block_index;
        u32 parent_dir_group_block_index;
        u32 group_block_offset;
        //t_inode* inode_parent_dir = NULL;
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
		inode_parent_dir = kmalloc(sizeof(t_inode));
                tot_group_block = ext2->superblock->s_blocks_count / ext2->superblock->s_log_block_size;
                //lookup_inode(parent_path,ext2,inode_parent_dir);
                parent_dir_group_block_index = (inode_parent_dir->i_number-1) / ext2->superblock->s_inodes_per_group;
                group_block_index = parent_dir_group_block_index;

		//read_group_block(ext2,group_block_index,group_block);                        
		inode_number = find_free_inode(group_block_index,ext2,0);
		if (inode_number == -1)
		{
                	while (group_block_index < tot_group_block && inode_number == -1)
                	{
				//read_group_block(ext2,(group_block_index + group_block_offset),group_block);                        
				inode_number = find_free_inode(group_block_index + group_block_offset,ext2,0);
                        	group_block_offset >>= 1;
                	}
                	if (inode_number == -1)
                	{
                        	group_block_index = parent_dir_group_block_index + 2;
                        	while(inode_number != -1 && group_block_index<tot_group_block)
                        	{
					//read_group_block(ext2,group_block_index,group_block);                    
					inode_number = find_free_inode(group_block_index,ext2,0);
					group_block_index++;
                        	}
                        	if (inode_number == -1)
                        	{
                                	group_block_index = 0;
                                	while(inode_number != -1 && group_block_index < parent_dir_group_block_index - 1)
                                	{
						//read_group_block(ext2,group_block_index,group_block);                      
						inode_number=find_free_inode(group_block_index,ext2,0);
                                	}
					group_block_index++;   
                        	}	
                	}
		}
		kfree(inode_parent_dir);
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
        if (inode_number != -1)
        {
                //write_group_block(ext2,group_block_index,group_block); da scommentare solo per test!!!!!!!!!!!!!!!!!!!!!!!!!!
		inode->i_number = inode_number;
        }
	return inode_number;
}

static int add_dir_entry(t_ext2* ext2,t_inode* inode_dir,u32 inode_number,char* filename,u32 type)
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

static int del_dir_entry(t_ext2* ext2,t_inode* inode_dir,u32 inode_number)
{
	u32 offset;
	u32 block_offset;
	u32 previous_rec_len;
	u32 found_entry;
	char* iob_dir;
	int ret_val;
	
	ret_val=-1;
	iob_dir=kmalloc(BLOCK_SIZE);
	offset=0;
	while (inode_dir->i_block[offset]<12 && !found_entry)
	{
		READ(BLOCK_SIZE/SECTOR_SIZE,inode_dir->i_block[offset],iob_dir);
		for (block_offset=0;block_offset<BLOCK_SIZE;block_offset++)
		{
			if (iob_dir[block_offset]==inode_number)
			{
				found_entry=1;
				break;
			}
		}
		previous_rec_len=block_offset;
		offset+=iob_dir[block_offset+4];
	}
	if (found_entry)
	{
		iob_dir[block_offset]=0;
		iob_dir[previous_rec_len+4]+=iob_dir[block_offset+4];
		WRITE(BLOCK_SIZE/SECTOR_SIZE,inode_dir->i_block[offset],iob_dir);
		ret_val=1;
	}
	return ret_val;
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
