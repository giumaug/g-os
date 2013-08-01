#ifndef EXT2_H                
#define EXT2_H

#include "system.h"
#include "data_types/primitive_types.h"

#define BLOCK_SIZE 1024
#define SECTOR_SIZE 512
#define NAME_MAX 50
#define PATH_MAX 1000
#define EXT2_N_BLOCKS 15
#define BLOCK_NUM(FILE_BLOCK_NUM) 

#define O_CREAT 0b1
#define O_APPEND 0b10
#define O_RDWR 0b100

#define BLOCK_SECTOR_ADDRESS(group_block_index,block)    ext2->partition_start_sector                                         \
							+(BLOCK_SIZE                                                          \
							+ext2->superblock->block_group_size*(group_block_index+1)             \
							+ext2->superblock->block_group_header_size+(BLOCK_SIZE*(block+1)))    \
							/SECTOR_SIZE;

#define ABSOLUTE_BLOCK_ADDRESS(group_block_index,relative_block_address) ext2->superblock->s_blocks_per_group*(group_block_index-1)+block

typedef struct s_superblock
{	
	u32 block_group_size;
	u32 block_group_header_size;
	//disk fields	
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
	
	u8 uuid[16];
	char volume_name[16];
	char last_mounted[64];
	u32 algorithm_usage_bitmap; 
	u8 prealloc_blocks;
	u8 prealloc_dir_blocks;
	u16 padding1;
	u32 reserved[204]; 
}
t_superblock;

typedef struct s_group_block
{
	//disk fields
	u32 bg_block_bitmap;
	u32 bg_inode_bitmap;
	u32 bg_inode_table; 
	u16 bg_free_blocks_count;
	u16 bg_free_inodes_count;
	u16 bg_used_dirs_count;
	u16 bg_pad;
	u32 bg_reserved[3]; 
}
t_group_block;

typedef struct s_inode
{
	u16 i_number;
	u32* indirect_block;
	u32 last_block_num;
	u32 last_file_block_num;
	u32 file_offset;
	u32 preallocated_block_count;
	u32 first_preallocated_block;

	//disk fields
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
	//union osd1;
	u32 osd1;
	u32 i_block[EXT2_N_BLOCKS];
	u32 i_generation; 
	u32 i_file_acl;
	u32 i_dir_acl;
	u32 i_faddr;
	//union osd2;
	u32 osd2_1;
	u32 osd2_2;
	u32 osd2_3;
}
t_inode;

struct s_device_desc;

typedef struct s_ext2
{
	t_superblock* superblock;
	u32 partition_start_sector;
	t_inode* root_dir_inode;
	struct s_device_desc* device_desc;
}
t_ext2;

#endif
