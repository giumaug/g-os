typedef struct s_superblock
{
	u32 inodes_count; 
	u32 blocks_count; 
	u32 r_blocks_count;
	u32 free_blocks_count;
	u32 free_inodes_count; 
	u32 first_data_block; 
	u32 log_block_size;
	u32 log_frag_size;
	u32 blocks_per_group;
	u32 frags_per_group;
	u32 inodes_per_group;
	u32 mtime;
	u32 wtime;
	u16 mnt_count;
	u16 max_mnt_count;
	u16 magic; 
	u16 state; 
	u16 errors;
	u16 minor_rev_level;
	u32 lastcheck;
	u32 checkinterval;
	u32 creator_os;	
	u32 rev_level;
	u16 def_resuid;
	u16 def_resgid;
	u32 first_ino;
	u16 inode_size;
	u16 block_group_nr;
	u32 feature_compat;
	u32 feature_incompat;
	u32 feature_ro_compat;
	u8[16] uuid;
	char[16] volume_name;
	char[64] last_mounted;
	u32 algorithm_usage_bitmap; 
	u8 prealloc_blocks;
	u8 prealloc_dir_blocks;
	u16 padding1;
	u32[204] reserved; 
}
t_superblock;

typedef struct s_group_block
{
	u32 bg_block_bitmap;
	u32 bg_inode_bitmap;
	u32 bg_inode_table; 
	u16 bg_free_blocks_count;
	u16 bg_free_inodes_count;
	u16 bg_used_dirs_count;
	u16 bg_pad;
	u32 [3] bg_reserved; 
}
t_group_block;

typedef struct s_ext2
{
	t_superblock superblock;
	t_group_block group_block;
 
}
t_ext2;

void init_ext2(t_ext2 *ext2)
{
	read_superblock(ext2->superblock);
	read_group_block(ext2->group_block);

}

void free_ext2()
{

}

void alloc_inode() 
{

}

void free_inode()
{

}

void alloc_block()
{

}

void free_block()
{

}

void static read_superblock(t_superblock *superblock)
{
	t_ata_request *ata_request;
	void *io_buffer;
	
	ata_request=kmalloc(sizeof(t_ata_request));
	io_buffer=kmalloc(512);
	
	ata_request->io_buffer=io_buffer;
	ata_request->lba=0;
	ata_request->sector_count=1;
	ata_request->process_context=NULL;
	ata_request->cmd=READ_28;

	_read_28_ata(ata_request,&processor_reg,FALSE);
	
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
	superblock->feature_compat=io_buffer[92];
	//u32
	superblock->feature_incompat=io_buffer[94];
	superblock->feature_ro_compat=io_buffer[98];
	//u8[16]	
	superblock->uuid=io_buffer[102];
	//u16
	superblock->volume_name=io_buffer[118];
	//u64
	superblock->last_mounted=io_buffer[120];
	//u32
	superblock->algorithm_usage_bitmap=io_buffer[184]; 
	//u8
	superblock->prealloc_blocks=io_buffer[188];         
	superblock->prealloc_dir_blocks=io_buffer[189];
	//u16
	superblock->padding1=io_buffer[190];
	//u32[204]
	superblock->reserved=io_buffer[192]; 
}

void static read_group_block(t_group_block *group_block)
{

}


