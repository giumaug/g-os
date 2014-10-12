#include "general.h"
#include "scheduler/process.h"
#include "ext2/ext2.h"
#include "virtual_memory/vm.h"
#include "memory_manager/kmalloc.h"
#include "elf_loader/elf_loader.h"

u32 load_elf_executable(char* path,struct t_process_context* process_context)
{
	int i;
	unsigned char* process_space;
	unsigned int fd;
	unsigned int process_size;
	Elf32_Ehdr* elf_header;
	Elf32_Phdr* elf_prg_header;
	Elf32_Shdr* elf_sct_header;
	t_ext2* ext2;

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

	for (i=0;i<elf_header->e_phnum;i++)
	{
		_read(ext2,fd,&elf_prg_header[i],sizeof(Elf32_Phdr));
		if (elf_prg_header[i].p_type==PT_LOAD)
		{
			process_size=elf_prg_header[i].p_memsz;
			//process_space=buddy_alloc_page(system.buddy_desc,process_size);
			process_space=buddy_alloc_page(system.buddy_desc,0x100000);//to fix!!!!!!!!!!!!!!!!!!!!
			process_context->phy_space_size=process_size;
			process_context->phy_add_space=FROM_VIRT_TO_PHY(process_space);
			_seek(ext2,fd,elf_prg_header[i].p_offset,SEEK_SET);
			_read(ext2,fd,process_space,elf_prg_header[i].p_memsz);
			break;
		}
	}
	
	kfree(elf_header);
	kfree(elf_prg_header);
	_close(ext2,fd);
	return 0;
}
