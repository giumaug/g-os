#include "scheduler/process.h"
#include "ext2/ext2.h"
#include "virtual_memory/vm.h"
#include "memory_manager/kmalloc.h"
#include "elf_loader/elf_loader.h"

//u32 __load_elf_executable(char* path,struct t_process_context* process_context)
//{
//	int i;
//	unsigned char* process_space;
//	unsigned int fd;
//	unsigned int process_size;
//	Elf32_Ehdr* elf_header;
//	Elf32_Phdr* elf_prg_header;
//	Elf32_Shdr* elf_sct_header;
//	t_ext2* ext2;
//
//	ext2=system.root_fs;
//	fd=_open(ext2,path,O_RDWR | O_APPEND);
//	if (fd==-1)
//	{
//		return -1;
//	}
//	
//	elf_header=kmalloc(sizeof(Elf32_Ehdr));
//	_read(ext2,fd,elf_header,sizeof(Elf32_Ehdr));
//	elf_prg_header=kmalloc(sizeof(Elf32_Phdr)*elf_header->e_phnum);
//	_seek(ext2,fd,elf_header->e_phoff,SEEK_SET);
//
//	for (i=0;i<elf_header->e_phnum;i++)
//	{
//		_read(ext2,fd,&elf_prg_header[i],sizeof(Elf32_Phdr));
//		if (elf_prg_header[i].p_type==PT_LOAD)
//		{
//			process_size=elf_prg_header[i].p_memsz;
//			process_space=buddy_alloc_page(system.buddy_desc,0x100000);//to fix!!!!!!!!!!!!!!!!!!!!
//			process_context->phy_space_size=process_size;
//			process_context->phy_add_space=FROM_VIRT_TO_PHY(process_space);
//			_seek(ext2,fd,elf_prg_header[i].p_offset,SEEK_SET);
//			_read(ext2,fd,process_space,elf_prg_header[i].p_memsz);
//			break;
//		}
//	}
//	
//	kfree(elf_header);
//	kfree(elf_prg_header);
//	_close(ext2,fd);
//	return 0;
//}

u32 elf_loader_init(t_elf_desc* elf_desc,char* path)
{
	int i;
	unsigned char* process_space;
	int fd;
	unsigned int process_size;
	Elf32_Ehdr* elf_header;
	Elf32_Phdr* elf_prg_header;
	Elf32_Shdr* elf_sct_header;
	t_ext2* ext2;

	void* xxx=elf_desc;

	ext2=system.root_fs;
	fd=_open(ext2,path,O_RDWR | O_APPEND);
	if (fd==-1)
	{
		return -1;
	}
	
	elf_header=kmalloc(sizeof(Elf32_Ehdr));
	_read(ext2,fd,elf_header,sizeof(Elf32_Ehdr));
	elf_prg_header=kmalloc(sizeof(Elf32_Phdr)*elf_header->e_phnum);
	_seek(ext2,fd,elf_header->e_phoff,SEEK_SET);
	elf_desc->file_desc=-1;

	for (i=0;i<elf_header->e_phnum;i++)
	{
		_read(ext2,fd,&elf_prg_header[i],sizeof(Elf32_Phdr));
		if (elf_prg_header[i].p_type==PT_LOAD)
		{
			elf_desc->file_desc=fd;
			elf_desc->file_offset=elf_prg_header[i].p_offset;
			elf_desc->file_size=elf_prg_header[i].p_memsz;
			break;
		}
	}
	kfree(elf_header);
	kfree(elf_prg_header);

	if (elf_desc->file_desc==-1)
	{
		return -1;
	}
	return 0;

}

u32 elf_loader_free(t_elf_desc* elf_desc)
{
	t_ext2* ext2;

	ext2=system.root_fs;
	_close(ext2,elf_desc->file_desc);
}

u32 elf_loader_read(t_elf_desc* elf_desc,u32 fault_addr,u32 page_addr)
{
	u32 offset;
	t_ext2* ext2;
	
	ext2=system.root_fs;

	offset=(fault_addr & ~(PAGE_SIZE-1)) - PROC_VIRT_MEM_START_ADDR + elf_desc->file_offset;
	_seek(ext2,elf_desc->file_desc,offset,SEEK_SET);
	_read(ext2,elf_desc->file_desc,page_addr,PAGE_SIZE);
}
