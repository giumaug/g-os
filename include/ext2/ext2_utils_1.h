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

void add_entry_to_dir(char* )
{
	int i;

	for (i=0;i<=11;i++)
	{
		if (parent_dir_inode->i_block[i]==0)
		{	
			break;
		} 
	}
	io_buffer=kmalloc(BLOCK_SIZE*(i+1));

	for (j=0;j<=(i-1);j++)
	{
		lba=FROM_BLOCK_TO_LBA(parent_dir_inode->i_block[j]);
		READ((BLOCK_SIZE/SECTOR_SIZE),lba,(io_buffer+BLOCK_SIZE*j));
	}
	QUI!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
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

u32 alloc_block(t_ext2* ext2,t_inode* i_node,u32 block_num)
{
        char* io_buffer;        
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
	
	block=0;
        preferred_block=0;
        discard_preallocated_block=1;
	group_block_index=(i_node->i_number-1)/ext2->superblock->s_inodes_per_group;
	group_block=kmalloc(sizeof(t_group_block));
	read_group_block(ext2,group_block_index,group_block);
	
        io_buffer=kmalloc(BLOCK_SIZE);
	read_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);

        if (block_num=i_node->last_file_block_num+1)
        {
		preferred_block=read_indirect_block(i_node,i_node->last_file_block_num+1); 
        }
        //block count start from 1
        else if (i_node->last_file_block_num!=0)
        {
                offset=block_num-1;
                while(preferred_block==0 && offset!=0)
                {
			preferred_block=read_indirect_block(i_node,block_num) == 0 ? 0 : block_num;
                        offset--;
                }
                if (preferred_block==0)
                {
			preferred_block=ABSOLUTE_BLOCK_ADDRESS(group_block_index,1);
                }
        }

        if(preferred_block==i_node->first_preallocated_block)
        {
                block=preferred_block;
                if (--i_node->preallocated_block_count>0)
                {
                        first_preallocated_block=i_node->first_preallocated_block++;
                        discard_preallocated_block=0;
                }
        }
       
        if (block==0)
        {
                buffer_byte=(preferred_block-1) / 8;
                byte_bit=(preferred_block-1) % 8;
                selected_bit=io_buffer[buffer_byte] & (2>>byte_bit);
                if (selected_bit==0)
                {
                        block=preferred_block;
                }
                else
                {
                        for (i=0;i<16;i++)
                        {
                                buffer_byte=(preferred_block-1+1+i) / 8;
                                byte_bit=(preferred_block-1+1+i) % 8;
                                selected_bit=io_buffer[buffer_byte] & (2>>byte_bit);
                                if (selected_bit==0)
                                {
                                        block=preferred_block+1+i;
                                        break;  
                                }
                        }
                }
                if (block==0)
                {
                        block=find_free_block(io_buffer,1);
                }
                if (block!=0)
                {
                       //nothing
                }
                else
                {
			for(i=0;i<ext2->superblock->s_blocks_count;i++)
                        {
				read_group_block(ext2,i,group_block);
				read_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);
                                block=find_free_block(io_buffer,1);
                                if (block!=0)
                                {
					group_block_index=i;
                                        break;
                                }
                        }
                }
        }
        if (block!=0)
        {
                if (discard_preallocated_block)
                {
                        free_block=0;                  
                        i_node->preallocated_block_count=8;
                        for(i=block;i<block+8;i++)
                        {
                                buffer_byte=i/8;
                                byte_bit=(preferred_block-1+1+i) % 8;
                                selected_bit=io_buffer[buffer_byte] & (2>>byte_bit);
                                if (selected_bit==0)
                                {
                                        free_block++;
                                }
                        }
                        if (free_block==8)
                        {
                                i_node->preallocated_block_count=8;
                                i_node->first_preallocated_block=block+1;              
                        }                      
                }
                io_buffer[buffer_byte]&= (255 & (2>>byte_bit));
                write_block_bitmap(ext2,group_block->bg_block_bitmap,io_buffer);
		group_block->bg_free_blocks_count--;
		write_group_block(ext2,group_block_index,group_block);
		write_indirect_block(i_node,block_num,block);
        }
	kfree(group_block);
        kfree(io_buffer);
	ext2->superblock->s_free_blocks_count--;
	i_node->last_file_block_num=block_num;
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

void find_parent_path(char* full_path,char* parent_path)
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
	}
	else if (index == 0 && full_path[0] == '.' && full_path[1] == '/')
	{
		parent_path[0] = '.';
		parent_path[1] = '/';
		parent_path[2] = '\0';	
	}
	else
	{
		path = full_path;
		i = 0;
		while(i < index)
		{
			parent_path[i] = full_path[i];
			i++;
		}
		parent_path[i] = '\0';
	}
}

void alloc_inode(char* parent_path,unsigned int type,t_ext2 *ext2, t_inode* inode)
{
        int inode_number;      
        char* current_byte = NULL;
        u32 i,j;        
        u32 group_block_index;
        u32 parent_dir_group_block_index;
        u32 group_block_offset;
        t_inode* inode_parent_dir = NULL;
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
                tot_group_block = ext2->superblock->s_blocks_count;
                lookup_inode(parent_path,ext2,inode_parent_dir);
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
                while(group_block_index < tot_group_block && inode_number == -1)
                {
			//read_group_block(ext2,group_block_index,group_block);                        
			inode_number = find_free_inode(group_block_index,ext2,1);
                }
        }
        if (inode_number != -1)
        {
                //write_group_block(ext2,group_block_index,group_block); da scommentare solo per test!!!!!!!!!!!!!!!!!!!!!!!!!!
		printk("selected inode is %d \n",inode->i_number);
		inode->i_number = inode_number;
        }
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
