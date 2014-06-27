#include<stdio.h>
#include<stdlib.h>

#define EI_NIDENT (16)
#define PT_LOAD	1
#define PT_PHDR	6

typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Word;
typedef	int  Elf32_Sword;
typedef unsigned int Elf32_Addr;
typedef unsigned int Elf32_Off;

typedef struct
{
  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
  Elf32_Half	e_type;			/* Object file type */
  Elf32_Half	e_machine;		/* Architecture */
  Elf32_Word	e_version;		/* Object file version */
  Elf32_Addr	e_entry;		/* Entry point virtual address */
  Elf32_Off	e_phoff;		/* Program header table file offset */
  Elf32_Off	e_shoff;		/* Section header table file offset */
  Elf32_Word	e_flags;		/* Processor-specific flags */
  Elf32_Half	e_ehsize;		/* ELF header size in bytes */
  Elf32_Half	e_phentsize;		/* Program header table entry size */
  Elf32_Half	e_phnum;		/* Program header table entry count */
  Elf32_Half	e_shentsize;		/* Section header table entry size */
  Elf32_Half	e_shnum;		/* Section header table entry count */
  Elf32_Half	e_shstrndx;		/* Section header string table index */
} Elf32_Ehdr;

typedef struct
{
  Elf32_Word	sh_name;		/* Section name (string tbl index) */
  Elf32_Word	sh_type;		/* Section type */
  Elf32_Word	sh_flags;		/* Section flags */
  Elf32_Addr	sh_addr;		/* Section virtual addr at execution */
  Elf32_Off	sh_offset;		/* Section file offset */
  Elf32_Word	sh_size;		/* Section size in bytes */
  Elf32_Word	sh_link;		/* Link to another section */
  Elf32_Word	sh_info;		/* Additional section information */
  Elf32_Word	sh_addralign;		/* Section alignment */
  Elf32_Word	sh_entsize;		/* Entry size if section holds table */
} Elf32_Shdr;

typedef struct
{
  Elf32_Word	p_type;			/* Segment type */
  Elf32_Off	p_offset;		/* Segment file offset */
  Elf32_Addr	p_vaddr;		/* Segment virtual address */
  Elf32_Addr	p_paddr;		/* Segment physical address */
  Elf32_Word	p_filesz;		/* Segment size in file */
  Elf32_Word	p_memsz;		/* Segment size in memory */
  Elf32_Word	p_flags;		/* Segment flags */
  Elf32_Word	p_align;		/* Segment alignment */
} Elf32_Phdr;

int _read(t_ext2* ext2,int fd, void* buf,u32 count)

load_executable(char* path) 
{
	int i;
	unsigned int section_entry;
	unsigned char* buffer;
	FILE* f;

	Elf32_Ehdr* elf_header;
	Elf32_Phdr* elf_prg_header;
	Elf32_Shdr* elf_sct_header;
	t_ext2* ext2;

	ext2=system.root_fs;
	elf_header=malloc(sizeof(Elf32_Ehdr));
	f=_open(ext2,path,O_RDWR | O_APPEND);
	
	fread(elf_header,sizeof(Elf32_Ehdr),1,f);
	elf_prg_header=malloc(sizeof(Elf32_Phdr)*elf_header->e_phnum);
	
	fseek(f,elf_header->e_phoff,SEEK_SET);
	for (i=0;i<elf_header->e_phnum;i++)
	{
		fread(&elf_prg_header[i],sizeof(Elf32_Phdr),1,f);
		if (elf_prg_header[i].p_type==PT_LOAD)
		{
			buffer=malloc(elf_prg_header[i].p_memsz);
			fseek(f,elf_prg_header[i].p_offset,SEEK_SET);
			fread(buffer,elf_prg_header[i].p_memsz,1,f);
			break;
		}
	}
	
	free(elf_header);
	free(elf_prg_header);
	free(buffer);
	fclose(f);
}

