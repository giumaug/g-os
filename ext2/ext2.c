
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

u32 alloc_inode(char* path,unsigned int type,struct t_processor_reg* processor_reg,t_ext2 *ext2) 
{
	u32 inode_number;	
	char* current_byte;
	u32 i,j;	
	u32 group_block_index;
	u32 parent_dir_group_block_index;
	u32 group_block_offset;	
	t_inode* i_node_parent_dir;
	t_group_block **group_block;
	void* io_buffer;
	u32 lba;
	u32 sector_count;

	// 1)seleziona inode parent dir
	// 2)seleziona 	group descriptor inode  (block group = (inode – 1) / INODES_PER_GROUP)
	// 3)se bg_free_inodes_count>0  else step 4)
	//	3.1)leggi blocco inode bitmap (bg_inode_bitmap)
	//	3.2)seleziona primo inode libero
	//	3.3)update file system
	//	3.4)ritorna inode 
	// 4)seleziona group descriptor group descriptor+1+2+4+.... inode mod(n=numero totale group descriptor)
	// 5)vai punto 3
	// 6)Seleziona primo group descriptor con inode libero a partire da group descriptor corrente +2
	if (type==0)
	{
		group_block_offset=0;
		inode_number=-1;
		tot_group_block=ext2->superblock->s_blocks_count/ext2->superblock->s_log_block_size;
		i_node_parent_dir=lookup_path(char* path);
		parent_dir_group_block_index=(i_node_parent_dir->i_number-1)/ext2->superblock->inodes_per_group;
		group_block_index=parent_dir_group_block_index;

		while (group_block_index<tot_group_block && inode_number==-1)
		{
			group_block=ext2->group_block[group_block_index];			
			inode_number=find_free_inode(t_group_block* group_block);
			group_block_index=group_block_offset>>1;
		}

		if (inode_number==-1)
		{
			group_block_index=parent_dir_group_block_index+2;
			while(inode_number!=-1 && group_block_index<tot_group_block)
			{
				group_block=ext2->group_block[group_block_index];			
				inode_number=find_free_inode(t_group_block* group_block);

			}
			if (inode_number==-1)
			{
				group_block_index=0;
				while(inode_number!=-1 && group_block_index<parent_dir_group_block_index-1)
				{
					group_block=ext2->group_block[group_block_index];			
					inode_number=find_free_inode(t_group_block* group_block);
				}	
			}
		}
	}

	//1)Seleziona primo group descriptor con numero inode<=media inode 
	//	2.1)Leggi blocco inode bitmap (bg_inode_bitmap)
	//	2.2)Seleziona primo inode libero
	//	2.3)Update file system	
	//	2.4)ritorna inode
	//2)Seleziona primo group descriptor con inode libero a partire da group descriptor corrente +1
	//3)Vai punto 2.1 
	else if (type==1)
	{
		ext2->superblock->block_free_inode_average;
		while (group_block_index<tot_group_block && inode_number==-1)
		{
			group_block=ext2->group_block[group_block_index];			
			inode_number=find_free_inode(t_group_block* group_block);
		}
	}
	
	if (inode_number!=-1)
	{
		_write_28_ata(sector_count,lba,io_buffer,processor_reg,current_process_context,TRUE);
	}
	return inode_number;
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
	t_group_block*  group_block;

	group_block_index=(i_node->i_number-1)/ext2->superblock->inodes_per_group;
	group_block=ext2->group_block[group_block_index];
	
	lba=ext2->partition_start_sector+group_block->bg_inode_bitmap/SECTOR_SIZE;
	sector_count=BLOCK_SIZE/SECTOR_SIZE;
	_read_28_ata(sector_count,lba,io_buffer,processor_reg,current_process_context,TRUE);

	inode_index = (i_node->i_number – 1) % ext2->superblock->s_blocks_per_group; 
	buffer_index=(inode_index-1) / 8;
	byte_bit=(inode_index-1) % 8;
	io_buffer[buffer_index]&= (255 & (2>>byte_bit));

	_write_28_ata(sector_count,lba,io_buffer,processor_reg,current_process_context,TRUE);
}

void alloc_block()
{

}

void free_block()
{

}

t_inode* lookup_path(char* path,t_ext2 *ext2)
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

void static read_superblock(t_ext2 *ext2)
{
	t_superblock *superblock;	
	t_ata_request *ata_request;
	void *io_buffer;
	
	superblock=kmalloc(sizeof(t_superblock));
	ext2->superblock=superblock;	
	ata_request=kmalloc(sizeof(t_ata_request));
	io_buffer=kmalloc(512);
	
	ata_request->io_buffer=io_buffer;
	ata_request->lba=1024+partition_start_sector;
	ata_request->sector_count=2;
	ata_request->process_context=NULL;
	ata_request->cmd=READ_28;

	_read_28_ata(ata_request,NULL,TRUE);
	
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
	superblock->mnt_count==io_buffer[52];         
	superblock->max_mnt_count==io_buffer[54];
	superblock->magic==io_buffer[56]; 
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
	kfree(io_buffer);
	kfree(ata_request);
}

read_group_block(t_ext2 *ext2)
{
	u32 block_in_group;
	u32 sector_count;
	t_group_block *group_block;

	block_group_nr=ext2->superblock->s_block_count/(8*ext2->superblock->s_log_block_size);	
	sector_count=block_group_nr*ext2->superblock->s_log_block_size/512;

	ata_request=kmalloc(sizeof(t_ata_request));
	io_buffer=kmalloc(512*sector_count);
	
	
	ata_request->io_buffer=io_buffer;
	ata_request->lba=2048+ext2->partition_start_sector;
	ata_request->sector_count=sector_count;
	ata_request->process_context=NULL;
	ata_request->cmd=READ_28;

	_read_28_ata(ata_request,NULL,TRUE);
	
	ext2->group_block=kmalloc(sizeof(t_group_block*)*block_group_nr);
	for (i=0;i<=block_group_nr;i++)
	{
		group_block=kmalloc(sizeof(t_group_block));
		ext2->group_block[i]=group_block;
		//u32 
		bg_block_bitmap=io_buffer[0+(23*i)];
	    	//u32 
		bg_inode_bitmap=io_buffer[4+(23*i)];
		//u32 
		bg_inode_table=io_buffer[8+(23*i)]; 
		//u16 
		bg_free_blocks_count=io_buffer[12+(23*i)];
		//u16 
		bg_free_inodes_count=io_buffer[14+(23*i)];
		//u16 
		bg_used_dirs_count=io_buffer[16+(23*i)];
		//u16 
		bg_pad=io_buffer[18+(23*i)];
		//u32[3] 
		bg_reserved=kmemcpy(&bg_reserved,&io_buffer[20+(23*i)],3); 			
	}
	kfree(io_buffer);
	kfree(ata_request);
}

u32 static lookup_partition(u8 partition_number)
{
	u32 first_partition_start_sector;	
	ata_request=kmalloc(sizeof(t_ata_request));
	io_buffer=kmalloc(512);
	
	ata_request->io_buffer=io_buffer;
	ata_request->lba=0;
	ata_request->sector_count=1;
	ata_request->process_context=NULL;
	ata_request->cmd=READ_28;

	_read_28_ata(ata_request,NULL,TRUE);
	first_partition_start_sector=io_buffer[0x1be+( partition_number*16)+8];

	kfree(io_buffer);
	kfree(ata_request);
	return first_partition_start_sector;
}

void static find_free_inode(t_group_block* group_block,t_ext2 *ext2)
{
	if (group_block->bg_free_inodes<=superblock->average_block_inode)
	{
		lba=ext2->partition_start_sector+group_block->bg_inode_bitmap/SECTOR_SIZE;
		sector_count=BLOCK_SIZE/SECTOR_SIZE;
		_read_28_ata(sector_count,lba,io_buffer,processor_reg,current_process_context,TRUE);

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
	}
	return inode_number;
}


