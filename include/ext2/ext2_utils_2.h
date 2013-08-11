static void* read_block_bitmap(u32 partition_start_sector,u32 bg_block_bitmap,void* io_buffer)
{
	u32 lba;
	u32 sector_count;
		
        lba=partition_start_sector+(bg_block_bitmap*BLOCK_SIZE/SECTOR_SIZE);
        sector_count=BLOCK_SIZE/SECTOR_SIZE;
	READ(device_desc,sector_count,lba,io_buffer);
}

static void* write_block_bitmap(u32 partition_start_sector,u32 bg_block_bitmap,void* io_buffer)
{
	u32 lba;
	u32 sector_count;
		
        lba=partition_start_sector+(bg_block_bitmap*BLOCK_SIZE/SECTOR_SIZE);
        sector_count=BLOCK_SIZE/SECTOR_SIZE;
	WRITE(sector_count,lba,io_buffer);
}

static u32 alloc_indirect_block(t_ext2* ext2,t_inode* i_node)
{
	u32 group_block_index;
	u32 indirect_block;
	u32 ret;
	t_group_block* group_block;
	void* io_buffer;

	ret=-1;	
	group_block_index=(i-node->i_number – 1)/ ext2->s_inodes_per_group;
	group_block=kmalloc(sizeof(t_group_block));
	read_group_block(ext2,group_block_index,group_block);
	io_buffer=kmalloc(BLOCK_SIZE);
	read_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);
	indirect_block=ABSOLUTE_BLOCK_ADDRESS(group_block_index,find_free_block(io_buffer,0));
	if (indirect_block!=0)
	{
		i_node->i_block[12]=indirect_block;
		write_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);
		write_group_block(ext2,group_block_index,group_block);
		ret=0;
	}
	else 
	{
		for(i=0;i<etx2->superblock->s_blocks_count;i++)
		{
			if (i!=group_block_index)
			{
				read_group_block(ext2,i,group_block);
				read_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);
				indirect_block=ABSOLUTE_BLOCK_ADDRESS(group_block_index,find_free_block(io_buffer,0));
				if (indirect_block!=0)
				{
					i_node->i_block[12]=indirect_block;
					write_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);
					write_group_block(ext2,group_block_index,group_block);
					ret=0;
				}
			}
		}
	}
	kfree(io_buffer);
	kfree(group_block);
	return ret;	       
}

static void free_indirect_block(t_i_node* i_node)
{
	u32 buffer_byte;
	u32 byte_bit;
	u32 group_block_index;
	u32 relative_block_address;
	t_group_block* group_block;
	void* io_buffer;

	group_block_index=i_node->i_block[12]/ext2->superblock->s_blocks_per_group;
	relative_block_address=i_node->i_block[12] % ext2->superblock->s_blocks_per_group;
	group_block=kmalloc(sizeof(t_group_block));
	read_group_block(ext2,group_block_index,group_block);
	io_buffer=kmalloc(BLOCK_SIZE);
	read_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);
	buffer_byte=relative_block_address/8;
	byte_bit=relative_block_address%8;
	io_buffer[buffer_byte]&= (255 &  ~(2>>byte_bit));
	write_block_bitmap(ext2->partition_start_sector,group_block->bg_block_bitmap,io_buffer);
	write_group_block(ext2,group_block_index,group_block);
	kfree(group_block);
	kfree(io_buffer);
}

static u32 read_indirect_block(t_inode* inode,u32 key)
{
	if (key>=0 && key<=11)
	{
		return i_node->i_block[key-1];
	}
	else
	{
		return i_node>indirect_block[key-12];
	} 
}

static void write_indirect_block(t_inode* inode,u32 key,u32 value)
{
	if (key>=0 && key<=11)
	{
		i_node->i_block[key-1]=value;
	}
	else
	{
		i_node>indirect_block[key-12]=value;
	}
}

void static read_superblock(t_superblock* superblock,partition_start_sector)
{
        void *io_buffer;
	
        io_buffer=kmalloc(512);
        READ(2,(1024+partition_start_sector),io_buffer);
       
        //u32
        superblock->inodes_count=io_buffer[0];        
        superblock->blocks_count=io_buffer[4];
        superblock->r_blocks_count=io_buffer[8];
        superblock->free_blocks_count=io_buffer[12];
        superblock->free_inodes_count=io_buffer[16];
        superblock->first_data_block=io_buffer[20];
        superblock->log_block_size=io_buffer[24];
        superblock->log_frag_size=io_buffer[28];
        superblock->blocks_per_group=io_buffer[32];
        superblock->frags_per_group=io_buffer[36];
        superblock->inodes_per_group=io_buffer[40];
        superblock->mtime=io_buffer[44];
        superblock->wtime=io_buffer[48];
        //u16
        superblock->mnt_count=io_buffer[52];        
        superblock->max_mnt_count=io_buffer[54];
        superblock->magic=io_buffer[56];
        superblock->state=io_buffer[58];
        superblock->errors=io_buffer[60];
        superblock->minor_rev_level=io_buffer[62];
        //u32
        superblock->lastcheck=io_buffer[64];                
        superblock->checkinterval=io_buffer[68];
        superblock->creator_os=io_buffer[72];  
        superblock->rev_level=io_buffer[76];
        //u16
        superblock->def_resuid=io_buffer[80];            
        superblock->def_resgid=io_buffer[82];
        //u32
        superblock->first_ino=io_buffer[84];                      
        //u16
        superblock->inode_size=io_buffer[88];
        superblock->block_group_nr=io_buffer[90];
        //u32
        superblock->feature_compat=io_buffer[92];
        superblock->feature_incompat=io_buffer[96];
        superblock->feature_ro_compat=io_buffer[100];
        //u8[16]
        kmemcpy(&superblock->uuid,&io_buffer[104],16);  
        //s8[16]
        kmemcpy(&superblock->volume_name,&io_buffer[120],16);
        //u64
        kmemcpy(&superblock->last_mounted,&io_buffer[136],64);
        //u32
        superblock->algorithm_usage_bitmap=io_buffer[200]);
        //u8
        superblock->prealloc_blocks=io_buffer[204];        
        superblock->prealloc_dir_blocks=io_buffer[205];
        //u16
        superblock->padding1=io_buffer[206];
        //u32[204]
        kmemcpy(&superblock->reserved,&io_buffer[208],204);
	superblock->block_group_header_size=3*BLOCK_SIZE
				    	   +32*superblock->s_blocks_count/superblock->s_blocks_per_group
				           +128*s_inodes_per_group;

	superblock->block_group_size=superblock->block_group_header_size+BLOCK_SIZE*superblock->s_blocks_per_group;
	kfree(io_buffer);
}

void static write_superblock(superblock,partition_start_sector)
{
        void *io_buffer;
	
        io_buffer=kmalloc(512);
   
        //u32
        io_buffer[0]=superblock->inodes_count;
        io_buffer[4]=superblock->blocks_count;
        io_buffer[8]=superblock->r_blocks_count;
        io_buffer[12]=superblock->free_blocks_count;
        io_buffer[16]=superblock->free_inodes_count;
        io_buffer[20]=superblock->first_data_block;
        io_buffer[24]=superblock->log_block_size;
        io_buffer[28]=superblock->log_frag_size;
        io_buffer[32]=superblock->blocks_per_group;
        io_buffer[36]=superblock->frags_per_group;
        io_buffer[40]=superblock->inodes_per_group;
        io_buffer[44]=superblock->mtime;
        io_buffer[48]=superblock->wtime;
        //u16
        io_buffer[52]=superblock->mnt_count;       
        io_buffer[54]=superblock->max_mnt_count;
        io_buffer[56]=superblock->magic;
        io_buffer[58]=superblock->state;
        io_buffer[60]=superblock->errors;
        io_buffer[62]=superblock->minor_rev_level;
        //u32
        io_buffer[64]=superblock->lastcheck;            
        io_buffer[68]=superblock->checkinterval;
        io_buffer[72]=superblock->creator_os;
        io_buffer[76]=superblock->rev_level;
        //u16
        io_buffer[80]=superblock->def_resuid;            
        io_buffer[82]=superblock->def_resgid;
        //u32
        io_buffer[84]=superblock->first_ino;                     
        //u16
        io_buffer[88]=superblock->inode_size;
        io_buffer[90]=superblock->block_group_nr;
        //u32
        io_buffer[92]=superblock->feature_compat;
        io_buffer[96]=superblock->feature_incompat;
        io_buffer[100]=superblock->feature_ro_compat;
        //u8[16]
	kmemcpy(&io_buffer[104],&superblock->uuid,16);
        //s8[16]
	kmemcpy(&io_buffer[120],&superblock->volume_name,16);
        //u64
	kmemcpy(&io_buffer[136],&superblock->last_mounted,64);
        //u32
        io_buffer[200])=superblock->algorithm_usage_bitmap;
        //u8
        io_buffer[204]=superblock->prealloc_blocks;    
        io_buffer[205]=superblock->prealloc_dir_blocks;
        //u16
        io_buffer[206]=superblock->padding1;
        //u32[204]
	kmemcpy(&io_buffer[208],&superblock->reserved,204);

	WRITE(2,(1024+partition_start_sector),io_buffer);	
	kfree(io_buffer);
}

write_group_block(t_ext2 *ext2,group_block_number,t_group_block* group_block)
{
	u32 sector_number;
	u32 sector_offset;
	u32 lba;
	void* io_buffer;

	//block_number start from 1
	sector_number=32*(block_number-1)/SECTOR_SIZE;
	sector_offset=(32*(block_number-1))%SECTOR_SIZE;
	lba=(2*BLOCK_SIZE)/SECTOR_SIZE+ext2->partition_start_sector+sector_number;
	io_buffer=kmalloc(512);

	//u32
	io_buffer[0+sector_offset]=group_block->bg_block_bitmap;
	//u32
	io_buffer[4+sector_offset]=group_block->bg_inode_bitmap;
	//u32
	io_buffer[8+sector_offset]=group_block->bg_inode_table;
	//u16
	io_buffer[12+sector_offset]=group_block->bg_free_blocks_count;
	//u16
	io_buffer[14+sector_offset]=group_block->bg_free_inodes_count;
	//u16
	io_buffer[16+sector_offset]=group_block->bg_used_dirs_count;
	//u16
	io_buffer[18+sector_offset]=group_block->bg_pad;
	//u32[3]
	kmemcpy(&io_buffer[20+sector_offset],group_block->bg_reserved,3);   
	
	WRITE(1,lba,io_buffer);
	free(io_buffer);
}

read_group_block(t_ext2 *ext2,group_block_number,t_group_block* group_block)
{
	u32 sector_number;
	u32 sector_offset;
	u32 lba;
	void* io_buffer;

	//block_number start from 1
	sector_number=32*(block_number-1)/SECTOR_SIZE;
	sector_offset=(32*(block_number-1))%SECTOR_SIZE;
	lba=(2*BLOCK_SIZE)/SECTOR_SIZE+ext2->partition_start_sector+sector_number;
	io_buffer=kmalloc(512);
	
	READ(1,lba,io_buffer);
	//u32
	group_block->bg_block_bitmap=io_buffer[0+sector_offset];
	//u32
	group_block->bg_inode_bitmap=io_buffer[4+sector_offset];
	//u32
	group_block->bg_inode_table=io_buffer[8+sector_offset];
	//u16
	group_block->bg_free_blocks_count=io_buffer[12+sector_offset];
	//u16
	group_block->bg_free_inodes_count=io_buffer[14+sector_offset];
	//u16
	group_block->bg_usread_inodeed_dirs_count=io_buffer[16+sector_offset];
	//u16
	group_block->bg_pad=io_buffer[18+sector_offset];
	//u32[3]
	kmemcpy(group_block->bg_reserved,&io_buffer[20+sector_offset],3);  
	
	free(io_buffer);
}

void static read_inode(t_ext2* ext2,t_inode* inode)
{
	u32 group_number;
	u32 group_offset;
	u32 inode_table_offset;
	u32 inode_offset;
	t_group_block* group_block;
	u32 lba;
	u32 sector_count;
	void* io_buffer;

	io_buffer=kmalloc(BLOCK_SIZE);	
	group_block=kmalloc(sizeof(t_group_block));
	
	group_number=inode_number/ext2->superblock->s_inodes_per_group; 
	group_offset=inode_number%ext2->superblock->s_inodes_per_group;
	read_group_block(ext2,group_block_index,group_block);

	inode_table_offset=group_offset/(BLOCK_SIZE/128);
	inode_offset=group_offset%(BLOCK_SIZE/128);	
	
	lba=group_block->bg_inode_table+inode_table_offset*(BLOCK_SIZE/SECTOR_SIZE);
	sector_count=BLOCK_SIZE/SECTOR_SIZE;
	READ(device_desc,sector_count,lba,io_buffer);

	//u16 
	inode->i_mode=io_buffer[inode_offset];
	//u16 
	inode->i_uid=io_buffer[inode_offset+2];
	//u32 
	inode->i_size=io_buffer[inode_offset+4];
	//u32 
	inode->i_atime=io_buffer[inode_offset+8];
	//u32 
	inode->i_ctime=io_buffer[inode_offset+12];
	//u32 
	inode->i_mtime=io_buffer[inode_offset+16];
	//u32 
	inode->i_dtime=io_buffer[inode_offset+20];
	//u16
	inode->i_gid=io_buffer[inode_offset+24];
	//u16 
	inode->i_links_count=io_buffer[inode_offset+26];
	//u32 
	inode->i_blocks=io_buffer[inode_offset+28];
	//u32 
	inode->i_flags=io_buffer[inode_offset+32];
	//u32 
	inode->osd1=io_buffer[inode_offset+36];
	//u32[EXT2_N_BLOCKS]
	inode->i_block[0]=io_buffer[inode_offset+40];
	inode->i_block[1]=io_buffer[inode_offset+44];
	inode->i_block[2]=io_buffer[inode_offset+48];
	inode->i_block[3]=io_buffer[inode_offset+52];
	inode->i_block[4]=io_buffer[inode_offset+56];
	inode->i_block[5]=io_buffer[inode_offset+60];
	inode->i_block[6]=io_buffer[inode_offset+64];
	inode->i_block[7]=io_buffer[inode_offset+68];
	inode->i_block[8]=io_buffer[inode_offset+72];
	inode->i_block[9]=io_buffer[inode_offset+76];
	inode->i_block[10]=io_buffer[inode_offset+80];
	inode->i_block[11]=io_buffer[inode_offset+84];
	inode->i_block[12]=io_buffer[inode_offset+88];
	inode->i_block[13]=io_buffer[inode_offset+92];
	inode->i_block[14]=io_buffer[inode_offset+96];
	//u32 
	inode->i_generation=io_buffer[inode_offset+100];
	//u32 
	inode->i_file_acl=io_buffer[inode_offset+104];
	//u32 
	inode->i_dir_acl=io_buffer[inode_offset+108];
	//u32 
	inode->i_faddr=io_buffer[inode_offset+112];
	//u32 
	inode->osd2_1=io_buffer[inode_offset+116];
	//u32 
	inode->osd2_2=io_buffer[inode_offset+120];
	//u32 
	inode->osd2_3=io_buffer[inode_offset+124];
	
	kfree(io_buffer);
	kfree(group_block);
}

void static write_inode(t_ext2* ext2,u32 inode_number,t_inode* inode)
{
	u32 group_number;
	u32 group_offset;
	u32 inode_table_offset;
	u32 inode_offset;
	t_group_block* group_block;
	u32 lba;
	u32 sector_count;
	void* io_buffer;

	io_buffer=kmalloc(BLOCK_SIZE);	
	group_block=kmalloc(sizeof(t_group_block));
	
	group_number=inode_number/ext2->superblock->s_inodes_per_group; 
	group_offset=inode_number%ext2->superblock->s_inodes_per_group;
	read_group_block(ext2,group_block_index,group_block);

	inode_table_offset=group_offset/(BLOCK_SIZE/128);
	inode_offset=group_offset%(BLOCK_SIZE/128);	
	
	lba=group_block->bg_inode_table+inode_table_offset*(BLOCK_SIZE/SECTOR_SIZE);
	sector_count=BLOCK_SIZE/SECTOR_SIZE;

	//u16 
	io_buffer[inode_offset]=inode->i_mode;
	//u16 
	io_buffer[inode_offset+2]=inode->i_uid;
	//u32 
	io_buffer[inode_offset+4]=inode->i_size;
	//u32 
	io_buffer[inode_offset+8]=inode->i_atime;
	//u32 
	io_buffer[inode_offset+12]=inode->i_ctime;
	//u32 
	io_buffer[inode_offset+16]=inode->i_mtime;
	//u32 
	io_buffer[inode_offset+20]=inode->i_dtime;
	//u16
	io_buffer[inode_offset+24]=inode->i_gid;
	//u16 
	io_buffer[inode_offset+26]=inode->i_links_count;
	//u32 
	io_buffer[inode_offset+28]=inode->i_blocks;
	//u32 
	io_buffer[inode_offset+32]=inode->i_flags;
	//u32 
	io_buffer[inode_offset+36]=inode->osd1;
	//u32[EXT2_N_BLOCKS]
	io_buffer[inode_offset+40]=inode->i_block[0];
	io_buffer[inode_offset+44]=inode->i_block[1];
	io_buffer[inode_offset+48]=inode->i_block[2];
	io_buffer[inode_offset+52]=inode->i_block[3];
	io_buffer[inode_offset+56]=inode->i_block[4];
	io_buffer[inode_offset+60]=inode->i_block[5];
	io_buffer[inode_offset+64]=inode->i_block[6];
	io_buffer[inode_offset+68]=inode->i_block[7];
	io_buffer[inode_offset+72]=inode->i_block[8];
	io_buffer[inode_offset+76]=inode->i_block[9];
	io_buffer[inode_offset+80]=inode->i_block[10];
	io_buffer[inode_offset+84]=inode->i_block[11];
	io_buffer[inode_offset+88]=inode->i_block[12];
	io_buffer[inode_offset+92]=inode->i_block[13];
	io_buffer[inode_offset+96]=inode->i_block[14];
	//u32 
	io_buffer[inode_offset+100]=inode->i_generation;
	//u32 
	io_buffer[inode_offset+104]=inode->i_file_acl;
	//u32 
	io_buffer[inode_offset+108]=inode->i_dir_acl;
	//u32 
	io_buffer[inode_offset+112]=inode->i_faddr;
	//u32 
	io_buffer[inode_offset+116]=inode->osd2_1;
	//u32 
	io_buffer[inode_offset+120]=inode->osd2_2;
	//u32 
	io_buffer[inode_offset+124]=inode->osd2_3;

	WRITE(sector_count,lba,io_buffer);
	kfree(io_buffer);
	kfree(group_block);
}

void static read_dir_inode(char* file_name,t_inode* parent_dir_inode,t_ext2* ext2,t_inode* inode)
{
	int i;
	int j;
	u32 i_number;
	u32 next_entry;
	u32 name_len;
	void* io_buffer;
	char file_name_entry[NAME_MAX];

	// For directory inode supposed max 12 block	
	for (i=0;i<=11;i++)
	{
		if (inode->i_block[i]==-1)
		{	
			break;
		} 
	}
	io_buffer=kmalloc(BLOCK_SIZE*(i+1));

	for (j=0;j<=i;j++)
	{
		READ(2,parent_dir_inode->i_block[i],(io_buffer+1024*j));
	}

	next_entry=0;	
	j=0;
	while(next_entry<=(i+1)*BLOCK_SIZE)
	{
		i_number=io_buffer[next_entry];
		next_entry=(u16) io_buffer[next_entry+4];
		name_len=(u8) io_buffer[next_entry+6];
		while(io_buffer[next_entry+6+j]==file_name[j] && j<name_len)		
		{
			j++;
		}
		if(name_len==j)
		{
			break;
		}	
	}
	if(next_entry<=(i+1)*BLOCK_SIZE)
	{
		inode->i_number=i_number;
		read_inode(t_ext2* ext2,inode)
	}
	else 
	{
		inode=NULL;
	}
	kfree(io_buffer);
}

u32 static lookup_partition(u8 partition_number)
{
        u32 first_partition_start_sector;      

        READ(sector_count,0,io_buffer,TRUE);
        first_partition_start_sector=io_buffer[0x1be+( partition_number*16)+8];
        kfree(io_buffer);
        return first_partition_start_sector;
}

u32 static find_free_inode(u32 group_block_index,t_ext2 *ext2,u32 check_threshold)
{
	u32 inode_number;
	t_group_block* group_block;

	inode_number=-1;
	group_block=kmalloc(sizeof(t_group_block));   
	read_group_block(ext2,group_block_index,group_block);                   
        if (group_block->bg_free_inodes<=(ext2->superblock->free_inodes_count/ext2->superblock->blocks_count)-1 || !check_threshold)
        {
                lba=ext2->partition_start_sector+group_block->bg_inode_bitmap/SECTOR_SIZE;
                sector_count=BLOCK_SIZE/SECTOR_SIZE;
		read(device_desc,sector_count,lba,io_buffer);

                while (inode_number!=-1 && i<BLOCK_SIZE)
                {
                        current_byte=*(io_buffer++);
                        while (inode_number!=-1 && j<8)
                        {
                                if (!(*current_byte & 2>>j)
                                {
                                        inode_number=i*8+j;
                                        *current_byte=*current_byte | 2>>j
                                }
                                j++;
                        }
                        i++;
                        j=0;
                }
		WRITE(sector_count,lba,io_buffer);
		group_block->bg_free_inodes_count--;
		write_group_block(ext2,group_block_index,group_block); 
		ext2->superblock->free_inodes_count--;
	}
	kfree(group_block);
        return inode_number;
}

u32 static find_free_block(void* io_buffer,u32 prealloc)
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
	
        for (i=1;i<=BLOCK_SIZE*8;i++)
        {
                buffer_byte=i/8;
                byte_bit=i % 8;
                selected_bit=io_buffer[buffer_byte]&=(2>>byte_bit);
                if (selected_bit==0)
                {
			if (!prealloc)
			{
				break;
			}
			else
			{
				free_block=0;
				for(j=i+1;j<=i+8;j++)
				{
					buffer_byte2=j/8;
					byte_bit2=j%8;
					selected_bit2=io_buffer[buffer_byte2]&=(2>>byte_bit2);
					if (selected_bit2==0)
					{
						free_block++;
					}
				}
				if (free_block==8)
				{
					break;
				}
			}    
        }
        io_buffer[buffer_byte]&= (255 & (2>>byte_bit));
        return i;      
}