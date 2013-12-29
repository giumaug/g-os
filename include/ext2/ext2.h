#ifndef EXT2_H                
#define EXT2_H

#include "system.h"
#include "general.h"
#include "data_types/primitive_types.h"
#include "data_types/hashtable.h"

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
							+ext2->superblock->s_block_group_size*(group_block_index+1)           \
							+ext2->superblock->s_block_group_header_size+(BLOCK_SIZE*(block+1)))  \
							/SECTOR_SIZE;

#define ABSOLUTE_BLOCK_ADDRESS(group_block_index,relative_block_address) ext2->superblock->s_blocks_per_group*(group_block_index-1)+relative_block_address

//#define WRITE(sector_count,lba,io_buffer) ext2->device_desc->write(ext2->device_desc,sector_count,lba,io_buffer)

#define WRITE(_sector_count,_lba,_io_buffer)     do {                                                                   \
						t_io_request io_request; 						\
					  	io_request.device_desc=ext2->device_desc;				\
						io_request.sector_count=_sector_count;					\
						io_request.lba=_lba;							\
						io_request.io_buffer=_io_buffer;					\
						io_request.process_context=system.process_info.current_process->val;	\
						ext2->device_desc->write(&io_request);                                  \
                                                } while(0);

//#define READ(sector_count,lba,io_buffer) ext2->device_desc->write(ext2->device_desc,sector_count,lba,io_buffer)

#define READ(_sector_count,_lba,_io_buffer)     do{                                                                     \
						t_io_request io_request; 						\
					  	io_request.device_desc=ext2->device_desc;				\
						io_request.sector_count=_sector_count;					\
						io_request.lba=_lba;							\
						io_request.io_buffer=_io_buffer;					\
						io_request.process_context=system.process_info.current_process->val;	\
						ext2->device_desc->read(&io_request);					\
						} while(0);

typedef struct s_superblock
{	
	u32 s_block_group_size;
	u32 s_block_group_header_size;
	//disk fields	
	u32 s_inodes_count; 
	u32 s_blocks_count; 
	u32 s_r_blocks_count;
	u32 s_free_blocks_count;
	u32 s_free_inodes_count; 
	u32 s_first_data_block; 
	u32 s_log_block_size;
	u32 s_log_frag_size;
	u32 s_blocks_per_group;
	u32 s_frags_per_group;
	u32 s_inodes_per_group;
	u32 s_mtime;
	u32 s_wtime;
	
	u16 s_mnt_count;
	u16 s_max_mnt_count;
	u16 s_magic; 
	u16 s_state; 
	u16 s_errors;
	u16 s_minor_rev_level;
	
	u32 s_lastcheck;
	u32 s_checkinterval;
	u32 s_creator_os;	
	u32 s_rev_level;
	
	u16 s_def_resuid;
	u16 s_def_resgid;
	
	u32 s_first_ino;
	
	u16 s_inode_size;
	u16 s_block_group_nr;
	
	u32 s_feature_compat;
	u32 s_feature_incompat;
	u32 s_feature_ro_compat;
	
	u8 s_uuid[16];
	char s_volume_name[16];
	char s_last_mounted[64];
	u32 s_algorithm_usage_bitmap; 
	u8 s_prealloc_blocks;
	u8 s_prealloc_dir_blocks;
	u16 s_padding1;
	u32 s_reserved[204]; 
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
