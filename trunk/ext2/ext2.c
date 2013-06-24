void init_ext2(t_ext2 *ext2)
{
        ext2>partition_start_sector=lookup_partition(1);        
        read_superblock(ext2->superblock,ext2->partition_start_sector);
        read_group_block(ext2->group_block);

}

void free_ext2()
{
        //remember to free all allocated memory!!!!!!!!
}

i_node* alloc_inode(char* path,unsigned int type,t_ext2 *ext2)
{
	u32 inode_number;

	inode_number=select_inode(path,type,ext2);
	i_node=kmaolloc(sizeof(t_inode));
	
		

}

void free_inode(t_inode* i_node,t_ext2 *ext2)
{
        u32 group_block_index;
        u32 lba;
        u32 sector_count;
        u32 inode_index;
        u32 buffer_index;
        u32 byte_bit;
        void* io_buffer;
	void* io_buffer2;
        t_group_block*  group_block;
	t_hashtable* group_hash;
	t_llist* block_list;
	t_llist_node* next;
	t_llist_node* sentinel;

	//1)find block descriptor conteinig inode	
	//2)clear inode_bitmap bit
	//3)clear_data_block_bitmap bit

        group_block_index=(i_node->i_number-1)/ext2->superblock->inodes_per_group;
        group_block=ext2->group_block[group_block_index];
       
        lba=ext2->partition_start_sector+(group_block->bg_inode_bitmap*BLOCK_SIZE/SECTOR_SIZE);
        sector_count=BLOCK_SIZE/SECTOR_SIZE;
        io_buffer=kmalloc(BLOCK_SIZE);
        _read_28_ata(sector_count,lba,io_buffer,TRUE); 
	
        inode_index = (i_node->i_number – 1) % ext2->superblock->s_blocks_per_group;
        buffer_index=(inode_index-1) / 8;
        byte_bit=(inode_index-1) % 8;
        io_buffer[buffer_index]&= (255 & (2>>byte_bit));

        _write_28_ata(sector_count,lba,io_buffer,processor_reg,current_process_context,TRUE);
        
	lba=ext2->partition_start_sector+i_node->i_block[12]*(BLOCK_SIZE/SECTOR_SIZE)-1;
	sector_count=BLOCK_SIZE/SECTOR_SIZE;
       	_read_28_ata(sector_count,lba,io_buffer,TRUE);

	group_hash=hashtable_init(50);
	group_list=new_dllist();
	fill_group_hash(group_hash,0,11,i_node);
	if (i_node->i_block[12]!=NULL)
	{
		fill_group_hash(block_list,group_list,group_hash,12,1033,i_node);
	}

	sentinel=ll_sentinel(group_list);
	next=ll_first(group_list);
	while (next!=sentinel) 
	{
		group_block_key=next->val;
		block_list=hashtable_get(group_hash,group_block_key))];

		group_block=ext2->group_block[block_index];
		read_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);
		
		sentinel2=ll_sentinel(block_list);
		next=ll_first(block_list);
		while (next!=sentinel) 
		{
			offset=next->val;
			buffer_byte=(offset-1) / 8;
                	byte_bit=byte_bit=(offset-1) % 8;
			io_buffer[buffer_index]&= (255 & (2>>byte_bit));
			next=ll_next(next);
		}
		write_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);
		free_llist(block_list);
	}
	if (i_node->i_block[12]!=NULL)
	{
		free_indirect_block(i_node);
	}
	hashtable_free(group_hash);
	free_llist(group_list);
	kfree(io_buffer);
}

u32 alloc_block(t_ext2* ext2,t_inode* i_node,u32 block_num)
{
        void* io_buffer;        
        u32 preferred_block;
        u32 lba;
        u32 block;
        u32 offset;
        u32 group_block;
	u32 group_block_index;
	u32 block_bitmap;
        u32 sector_count;
        u32 free_block;
	u32 block_sector;
        u32 i;
	
	block=0;
        preferred_block=0;
        discard_preallocated_block=1;
        group_block_index=(i-node->i_number – 1)/ ext2->s_inodes_per_group;
	group_block=ext2->group_block[group_block_index];
	
        io_buffer=kmalloc(BLOCK_SIZE);
	read_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);

        if (block_num=ext2->last_file_block_num+1)
        {
                preferred_block=ext2->last_file_block_num+1;
        }
        //block count start from 1
        else if (ext2->last_file_block_num!=0)
        {
                offset=block_num-1;
                while(preferred_block==0 && offset!=0)
                {
			preferred_block=read_indirect_block(i_node,block_num) == 0 ? 0 : block_num;
                        offset--;
                }
                if (preferred_block==0)
                {
                        preferred_block=1;
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
                selected_bit=io_buffer[buffer_byte]&=(2>>byte_bit);
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
                                selected_bit=io_buffer[buffer_byte]&=(2>>byte_bit);
                                if (selected_bit==0)
                                {
                                        block=preferred_block+1+i;
                                        break;  
                                }
                        }
                }
                if (block==0)
                {
                        block=find_free_block(io_buffer,i_node,1);
                }
                if (block!=0)
                {
                       //nothing
                }
                else
                {
			for(i=0;i<etx2->superblock->s_blocks_count;i++)
                        {
				group_block=ext2->group_block[i];
				read_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);
                                block=find_free_block(io_buffer,i_node,1);
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
                                selected_bit=io_buffer[buffer_byte]&=(2>>byte_bit);
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
                write_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);
		write_indirect_block(inode,block_num,block);
        }
        kfree(io_buffer);
	return BLOCK_SECTOR_ADDRESS(group_block_index,block);
}

void free_block()
{
	//nothing
}

t_inode* lookup_path(char* path,t_ext2* ext2)
{
        int i,j;
        t_inode* parent_dir_inode;
        char[NAME_MAX] name;
               
        if (path[0]=='/')
        {                                
                parent_dir_inode=ext2->root_dir_inode;
                i=1;
        }
        else if(path[0]=='.' && path[1]=='/')
        {
                parent_dir_inode=system.process_info.current_process->current_dir_inode;
                i=2;    
        }

        while(path[i]!='\\' && path[i]!='\0')
        {
                j=0;
                while((path[i]!='\')
                {
                        name[j++]=path[i++];    
                }
                parent_dir_inode=lookup_inode(parent_dir_inode,name);  
        }
        return parent_dir_inode;
}








        



