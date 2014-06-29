#include "elf_loader/elf_loader.h"

extern t_system system;

void load_elf_executable(char* path,struct t_process_context* process_context) 
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
	elf_header=_malloc(sizeof(Elf32_Ehdr));
	fd=_open(ext2,path,O_RDWR | O_APPEND);
	_read(ext2,fd,elf_header,sizeof(Elf32_Ehdr));
	elf_prg_header=_malloc(sizeof(Elf32_Phdr)*elf_header->e_phnum);
	_fseek(fd,elf_header->e_phoff,SEEK_SET);

	for (i=0;i<elf_header->e_phnum;i++)
	{
		_read(ext2,fd,&elf_prg_header[i],sizeof(Elf32_Phdr));
		if (elf_prg_header[i].p_type==PT_LOAD)
		{
			process_size=elf_prg_header[i].p_memsz;
			process_space=buddy_alloc_page(&system.buddy_desc,process_size);
			current_process_context->phy_space_size=process_size;
			current_process_context->phy_add_space=FROM_VIRT_TO_PHY(process_space);
			_fseek(f,elf_prg_header[i].p_offset,SEEK_SET);
			_read(ext2,fd,process_space,elf_prg_header[i].p_memsz);
			break;
		}
	}
	
	free(elf_header);
	free(elf_prg_header);
	_close(fd);
}
