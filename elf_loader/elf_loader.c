#include "scheduler/process.h"
#include "ext2/ext2.h"
#include "virtual_memory/vm.h"
#include "memory_manager/kmalloc.h"
#include "elf_loader/elf_loader.h"

u32 elf_loader_init(t_elf_desc* elf_desc,char* path)
{
	int i;
 	unsigned char* process_space = NULL;
	int fd;
	unsigned int process_size;
	Elf32_Ehdr* elf_header = NULL;
	Elf32_Phdr* elf_prg_header = NULL;
	Elf32_Shdr* elf_sct_header = NULL;
	t_ext2* ext2 = NULL;
	t_stat* stat = NULL;

	elf_desc->file_desc = -1;
	ext2 = system.root_fs;
	stat = kmalloc(sizeof(t_stat));

	_stat(ext2,path,stat);
	if ((stat->st_mode & 0x8000) != 0x8000)
	{
		kfree(stat);
		return -1;
	}

	fd = _open(ext2,path,O_RDWR | O_APPEND);
	if (fd == -1)
	{
		kfree(stat);
		return -1;
	}
	
	elf_header = kmalloc(sizeof(Elf32_Ehdr));
	_read(ext2,fd,elf_header,sizeof(Elf32_Ehdr),0);
	elf_prg_header = kmalloc(sizeof(Elf32_Phdr)*elf_header->e_phnum);
	_seek(ext2,fd,elf_header->e_phoff,SEEK_SET);

	for (i = 0;i < elf_header->e_phnum;i++)
	{
		_read(ext2,fd,&elf_prg_header[i],sizeof(Elf32_Phdr),0);
		if (elf_prg_header[i].p_type == PT_LOAD)
		{
			elf_desc->file_desc = fd;
			elf_desc->file_offset = elf_prg_header[i].p_offset;
			elf_desc->file_size = elf_prg_header[i].p_memsz;
			break;
		}
	}
	kfree(stat);
	kfree(elf_header);
	kfree(elf_prg_header);
	if (elf_desc->file_desc == -1)
	{
		return -1;
	}
	return 0;
}

u32 elf_loader_free(t_elf_desc* elf_desc)
{
	t_ext2* ext2 = NULL;

 	ext2 = system.root_fs;
	if (elf_desc->file_desc != -1)
	{
		_close(ext2,elf_desc->file_desc);
	}
}

u32 elf_loader_read(t_elf_desc* elf_desc,u32 fault_addr,u32 page_addr)
{
	u32 offset;
	t_ext2* ext2 = NULL;
	
	ext2 = system.root_fs;
	offset = (fault_addr & (~(PAGE_SIZE-1))) - PROC_VIRT_MEM_START_ADDR + elf_desc->file_offset;
	_seek(ext2,elf_desc->file_desc,offset,SEEK_SET);
	_read(ext2,elf_desc->file_desc,page_addr,PAGE_SIZE,0);
}
