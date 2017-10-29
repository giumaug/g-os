#ifndef EXT2_H                
#define EXT2_H

#include "system.h"

#define READ_BYTE(src,dst) do {unsigned short* p;p=src;dst=(*p)&0xff;} while(0);
#define READ_WORD(src,dst) do {unsigned short* p;p=src;dst=*p;} while(0);
#define READ_DWORD(src,dst) do {unsigned int* p;p=src;dst=*p;} while(0);

#define BLOCK_SIZE 1024
#define SECTOR_SIZE 512
#define NAME_MAX 50
#define PATH_MAX 1000
#define EXT2_N_BLOCKS 15
#define BLOCK_NUM(FILE_BLOCK_NUM) 
#define ROOT_INODE 2

#define O_CREAT 0b1
#define O_APPEND 0b10
#define O_RDWR 0b100

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define INDIRECT_0_LIMIT  11
#define INDIRECT_1_LIMIT  ((BLOCK_SIZE / 4) + 11)
#define INDIRECT_2_LIMIT  ((BLOCK_SIZE / 4)*(BLOCK_SIZE / 4) + (BLOCK_SIZE / 4) + 11)
#define INDIRECT_3_LIMIT  (BLOCK_SIZE / 4)*(BLOCK_SIZE / 4)*(BLOCK_SIZE / 4) + (BLOCK_SIZE / 4)*(BLOCK_SIZE / 2) + (BLOCK_SIZE / 4)+11

#define FROM_BLOCK_TO_LBA(block_num) ext2->partition_start_sector+block_num*BLOCK_SIZE/SECTOR_SIZE

#define BLOCK_SECTOR_ADDRESS(group_block_index,block)    ext2->partition_start_sector                                         \
							+(BLOCK_SIZE                                                          \
							+ext2->superblock->s_block_group_size*(group_block_index+1)           \
							+ext2->superblock->s_block_group_header_size+(BLOCK_SIZE*(block+1)))  \
							/SECTOR_SIZE;

#define ABSOLUTE_BLOCK_ADDRESS(group_block_index,relative_block_address) ext2->superblock->s_blocks_per_group*(group_block_index-1)+relative_block_address

#define WRITE(_sector_count,_lba,_io_buffer)    do {                                                                    \
						t_io_request* io_request; 						\
                                                io_request=kmalloc(sizeof(t_io_request));                               \
					  	io_request->device_desc=ext2->device_desc;				\
						io_request->sector_count=_sector_count;					\
						io_request->lba=_lba;							\
						io_request->io_buffer=_io_buffer;					\
						io_request->process_context=system.process_info->current_process->val;	\
						ext2->device_desc->write(io_request);                                   \
						kfree(io_request);                                                      \
                                                } while(0);                                       

#define READ(_sector_count,_lba,_io_buffer)     do{                                                                     \
						t_io_request* io_request; 						\
                                                io_request=kmalloc(sizeof(t_io_request));                               \
					  	io_request->device_desc=ext2->device_desc;				\
						io_request->sector_count=_sector_count;					\
						io_request->lba=_lba;							\
						io_request->io_buffer=_io_buffer;					\
						io_request->process_context=system.process_info->current_process->val;	\
						ext2->device_desc->read(io_request); 					\
                                                kfree(io_request);                                                      \
						} while(0);

#define P_WRITE(_sector_count,_lba,_io_buffer)  do{                                                                     \
						t_io_request* io_request;                                               \
                                                io_request=kmalloc(sizeof(t_io_request));                               \
					  	io_request->device_desc=ext2->device_desc;				\
						io_request->sector_count=_sector_count;					\
						io_request->lba=_lba;							\
						io_request->io_buffer=_io_buffer;					\
						ext2->device_desc->p_write(io_request);                                 \
						kfree(io_request);                                                      \
                                                } while(0);

#define P_READ(_sector_count,_lba,_io_buffer)   do{                                                                     \
						t_io_request* io_request;         					\
						io_request=kmalloc(sizeof(t_io_request));                               \
					  	io_request->device_desc=ext2->device_desc;				\
						io_request->sector_count=_sector_count;					\
						io_request->lba=_lba;							\
						io_request->io_buffer=_io_buffer;					\
						ext2->device_desc->p_read(io_request); 					\
						kfree(io_request);                                                      \
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

typedef struct s_indirect_block
{
	char* block;
	struct s_indirect_block** block_map;
}
t_indirect_block;

typedef struct s_inode
{
	u16 i_number;
//	u32* indirect_block;
	u32 last_block_num;
	u32 last_file_block_num;
	u32 file_offset;
	u32 preallocated_block_count;
	u32 first_preallocated_block;
	struct s_indirect_block* indirect_block_1;
	struct s_indirect_block* indirect_block_2;

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

struct s_stat 
{             
	u16 st_ino;
	u16 st_mode;
	u16 st_uid;
	u16 st_gid;
	u32 st_size;
	u32 st_atime;
	u32 st_mtime;
	u32 st_ctime;
}
typedef t_stat;

struct s_device_desc;

typedef struct s_ext2
{
	t_superblock* superblock;
	u32 partition_start_sector;
	//t_inode* root_dir_inode;
	struct s_device_desc* device_desc;
}
t_ext2;

void init_ext2(t_ext2 *ext2,struct s_device_desc* device_desc);
void free_ext2(t_ext2* ext2);
int _open(t_ext2* ext2,const char* fullpath, int flags);
int _close(t_ext2* ext2,int fd);
int _read(t_ext2* ext2,int fd, void* buf,u32 count);
int _write(t_ext2* ext2,int fd, const void *buf,u32 count);
int _seek(t_ext2* ext2,int fd,unsigned int offset,int whence);
int _rm(t_ext2* ext2,char* fullpath);
int _mkdir(t_ext2* ext2,const char* fullpath);
int _chdir(t_ext2* ext2,char* path);
int _stat(t_ext2* ext2,char* pathname,t_stat* stat);
t_inode* inode_clone(t_inode* inode);
t_inode* inode_init();
int inode_free(t_inode* inode);
t_indirect_block* clone_indirect_block(t_indirect_block* indirect_block);
t_hashtable* clone_file_desc(t_hashtable* file_desc);
void _break();
void _read_test(t_ext2* ext2);

#endif
