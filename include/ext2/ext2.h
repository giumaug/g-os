#ifndef EXT2_H                
#define EXT2_H

#include "system.h"

//#define READ_BYTE(src, dst) do { unsigned short* p; p = src; dst = ( *p ) & 0xff; } while(0);
#define READ_BYTE(src, dst) do { unsigned char* p; p = src; dst = ( *p ); } while(0);
#define READ_WORD(src, dst) do { unsigned short* p; p = src; dst = *p; } while(0);
#define READ_DWORD(src, dst) do { unsigned int* p; p = src; dst = *p; } while(0);

#define WRITE_BYTE(src, dst) do { unsigned char* p; p = dst; *p = src; } while(0);
#define WRITE_WORD(src, dst) do { unsigned short* p; p = dst; *p = src; } while(0);
#define WRITE_DWORD(src, dst) do { unsigned int* p; p = dst; *p = src; } while(0);

#define BLOCK_SIZE 1024
#define SECTOR_SIZE 512
#define NAME_MAX 50
#define PATH_MAX 1000
#define EXT2_N_BLOCKS 15
#define BLOCK_NUM(FILE_BLOCK_NUM) 
#define ROOT_INODE 2
#define MOUNT_MAP_SIZE 10
#define PREALLOCATED_BLOCKS 8

#define O_CREAT 0b1
#define O_APPEND 0b10
#define O_RDWR 0b100

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define FILE      1
#define DIRECTORY 2

#define INODE_CACHE_SIZE 1000

#define INDIRECT_0_LIMIT  11
#define INDIRECT_1_LIMIT  ((BLOCK_SIZE / 4) + 11)
#define INDIRECT_2_LIMIT  ((BLOCK_SIZE / 4)*(BLOCK_SIZE / 4) + (BLOCK_SIZE / 4) + 11)
#define INDIRECT_3_LIMIT  (BLOCK_SIZE / 4)*(BLOCK_SIZE / 4)*(BLOCK_SIZE / 4) + (BLOCK_SIZE / 4)*(BLOCK_SIZE / 2) + (BLOCK_SIZE / 4)+11

#define FROM_BLOCK_TO_LBA(block_num) ext2->partition_start_sector + block_num * BLOCK_SIZE / SECTOR_SIZE

#define BLOCK_SECTOR_ADDRESS(group_block_index,block)    ext2->partition_start_sector                                         \
							+(BLOCK_SIZE                                                          \
							+ext2->superblock->s_block_group_size*(group_block_index+1)           \
							+ext2->superblock->s_block_group_header_size+(BLOCK_SIZE*(block+1)))  \
							/SECTOR_SIZE;

#define ABSOLUTE_BLOCK_ADDRESS(group_block_index,relative_block_address) ext2->superblock->s_blocks_per_group * group_block_index +     relative_block_address

#define RELATIVE_BLOCK_ADDRESS(group_block_index,absolute_block_address) absolute_block_address - ext2->superblock->s_blocks_per_group * group_block_index

#define RELATIVE_BITMAP_BYTE(block_index, blocks_per_group) ((block_index - 1) % blocks_per_group) / 8
#define RELATIVE_BITMAP_BIT(block_index, blocks_per_group) ((block_index - 1) % blocks_per_group) % 8

#define TOT_FS_GROUP(tot_fs_block, block_per_group) (tot_fs_block / block_per_group) == 0 ? \
                                                    (tot_fs_block / block_per_group) :      \
                                                    (tot_fs_block / block_per_group) + 1;

#define FIRST_DATA_BLOCK(ext2,group_block)  (((ext2->superblock->s_inodes_per_group * 128) % BLOCK_SIZE) == 0 ?   \
			                 ((ext2->superblock->s_inodes_per_group * 128) / BLOCK_SIZE) :           \
				         ((ext2->superblock->s_inodes_per_group * 128) / BLOCK_SIZE) + 1) +       \
                                         group_block->bg_inode_table

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

#define READ_DMA(_sector_count,_lba,_io_buffer) do{                                                                     \
						t_io_request* io_request; 						\
                                                io_request=kmalloc(sizeof(t_io_request));                               \
					  	io_request->device_desc=ext2->device_desc;				\
						io_request->sector_count=_sector_count;					\
						io_request->lba=_lba;							\
						io_request->io_buffer=_io_buffer;					\
						io_request->process_context=system.process_info->current_process->val;	\
						ext2->device_desc->read_dma(io_request); 				\
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

#define WRITE_DMA(_sector_count,_lba,_io_buffer) do{                                                                     \
						t_io_request* io_request; 						\
                                                io_request=kmalloc(sizeof(t_io_request));                               \
					  	io_request->device_desc=ext2->device_desc;				\
						io_request->sector_count=_sector_count;					\
						io_request->lba=_lba;							\
						io_request->io_buffer=_io_buffer;					\
						io_request->process_context=system.process_info->current_process->val;	\
						ext2->device_desc->write_dma(io_request); 				\
                                                kfree(io_request);                                                      \
						} while(0);

#define ENTRY_PAD(entry_len)  ((entry_len % 4) != 0 ?  4 - (entry_len % 4) : 0)

struct s_file;
struct s_inode_cache;
typedef struct s_superblock
{
	struct s_ext2* ext2;	
	u32 s_block_group_size;
	u32 s_block_group_header_size;
	char** group_block_list;
	char* group_block_list_status;
    char** group_block_bitmap_list;
	char* group_block_bitmap_list_status;
	char** group_inode_bitmap_list;
	char* group_inode_bitmap_list_status;
    struct s_inode_cache* inode_cache;
        
	u8 group_block_num;
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
	u32 s_reserved[816]; 
}
t_superblock;

typedef struct s_group_block
{
	struct s_ext2* ext2;
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
	u32* block;
	struct s_indirect_block** block_map;
}
t_indirect_block;

//IMPORTANT CONSIDERATION:INODE IS CONSIDERED A SHARED RESOURCE AMONG PROCESS.
//SOME FILEDS ARE HOWEVER PER PROCESS I.E:FILE_OFFSET.WE NEED TO DECOUPLE INODE
//IN TWO STRCTURES LIKE FILE_OBJECT AND INODE IN LINUX.FOR THE MOMENT RACE 
//CONDITION ON PER PROCESS FILEDS LIKE FILE_OFFSET MUST BE MANAGED ON USER SPACE.
typedef struct s_inode
{
	u8 status;
	u32 counter;
	u16 i_number;
	u32 last_block_num;
	s32 last_file_block_num;
	//u32 file_offset;
	u8 preallocated_block_count;
	s32 first_preallocated_block;
	struct s_indirect_block* indirect_block_1;
	struct s_indirect_block* indirect_block_2;
	//struct s_inode* parent_dir;
	struct s_ext2* ext2;

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
	u32 osd1;
	u32 i_block[EXT2_N_BLOCKS];
	u32 i_generation; 
	u32 i_file_acl;
	u32 i_dir_acl;
	u32 i_faddr;
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
	struct s_sem_desc* sem;
}
t_ext2;

typedef struct s_mount
{
	t_inode* inode;
	t_ext2* ext2;
}	
t_mount_point;

typedef struct s_block_disk
{
	u32 lba;
	char* data;
}
t_block_disk;

u32 static inline  __attribute__((always_inline)) bitmap_block(u32 tot_fs_block, u32 block_per_group, u32 group_block_index)
{
	u32 block_count;

	if (BLOCK_SIZE * (group_block_index + 1) > tot_fs_block)
	{
		block_count = (tot_fs_block % block_per_group) * 8;
	}
	else
	{
		block_count = BLOCK_SIZE * 8;
	}
	return block_count;
}

void init_ext2(t_ext2 *ext2,struct s_device_desc* device_desc);
void free_ext2(t_ext2* ext2);
int _open(t_ext2* ext2,const char* fullpath, int flags);
int _close(t_ext2* ext2,int fd);
int _read(t_ext2* ext2,int fd, void* buf,u32 count,u8 is_dma);
int _write(t_ext2* ext2,int fd, const void *buf,u32 count);
int _seek(t_ext2* ext2,int fd,unsigned int offset,int whence);
int _rm(t_ext2* ext2,char* fullpath);
int _mkdir(t_ext2* ext2,const char* fullpath);
int _chdir(t_ext2* ext2,char* path);
int _stat(t_ext2* ext2,char* pathname,t_stat* stat);
t_inode* inode_clone(t_inode* inode);
t_inode* inode_init(t_ext2* ext2);
int inode_free(t_inode* inode);
int inode_free_indirect_block(t_inode* inode);
t_indirect_block* clone_indirect_block(t_indirect_block* indirect_block);
t_hashtable* clone_file_desc(t_hashtable* file_desc);
void flush_inode_cache(t_ext2* ext2);
struct s_file* file_init(t_inode* inode);
int file_free(struct s_file* file);

void _break();
void _read_test(t_ext2* ext2);


#endif
