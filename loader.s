.global loader                  # making entry point visible to linker
.global MULTIBOOT_INFO
.global MULTIBOOT_MAGIC

.section .multiboot

# setting up the Multiboot header - see GRUB docs for details
.set MODULEALIGN,	1<<0                   # align loaded modules on page boundaries
.set MEMINFO,      	1<<1                   # provide memory map
.set FLAGS,        	MODULEALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,        	0x1BADB002             # 'magic number' lets bootloader find the header
.set CHECKSUM,     	-(MAGIC + FLAGS)       # checksum required

# not used, garbage padding values.E85250D6
.set HEADER_ADDR, 		0
.set LOAD_ADDR, 	    0
.set LOAD_END_ADDR, 	0
.set BSS_END_ADDR, 		0
.set ENTRY_ADDR, 		0

# RGB: linear graphics mode?
.set MODE_TYPE,			0
# no preference
.set WIDTH,				0
.set HEIGHT,			0
.set DEPTH,				0

.align 4
MultiBootHeader:
.long MAGIC
.long FLAGS
.long CHECKSUM
.long HEADER_ADDR
.long LOAD_ADDR
.long LOAD_END_ADDR
.long BSS_END_ADDR
.long ENTRY_ADDR
.long MODE_TYPE
.long WIDTH
.long HEIGHT
.long DEPTH

.section .bss
# only first align works (bug?)
.align 0x1000
.lcomm PAGE_TABLE,0x100000
#.align 0x1000
.lcomm PAGE_DIR,0x400
#.align 0x1000
.lcomm CURRENT_PAGE,0x4
.lcomm PADD,0xBF8
#.align 0x1000
.lcomm TMP_PAGE_TABLE,0x1000
.lcomm TMP_PAGE_TABLE_1,0x1000

.section .text

.set PAGE_DIR_OFFSET_VAL, 0xc8     # 50 page table for 200M 256 for 1G
.set KERNEL_PHY_ADD, 0x100000
.set START_LOADER_PHY_ADD, 0xC00000
.set END_LOADER_PHY_ADD, 0xE00000

MULTIBOOT_INFO:
	.long 0

MULTIBOOT_MAGIC:
	.long 0

PAGE_DIR_OFFSET:
      .long PAGE_DIR_OFFSET_VAL+4

PAGE_TABLE_OFFSET:
      .long 0
 
loader:
	cli
	mov %ebx, MULTIBOOT_INFO
	mov %eax, MULTIBOOT_MAGIC
# inizialize virtual memory
	mov PAGE_DIR_OFFSET,%eax
	mov $PAGE_DIR,%edx
	add %eax,%edx
	add $0xC00,%edx 
	mov %edx,PAGE_DIR_OFFSET
	mov $PAGE_TABLE,%eax
	mov %eax,CURRENT_PAGE
	mov $KERNEL_PHY_ADD,%ebx
	mov $PAGE_DIR,%edx
	add $0xC00,%edx
	mov CURRENT_PAGE,%eax
fill_directory:	
    add $0b111,%eax
	mov %eax,(%edx)
	mov CURRENT_PAGE,%eax
	add $0x1000,%eax
	mov %eax,PAGE_TABLE_OFFSET
	mov CURRENT_PAGE,%eax
fill_table:
	mov %ebx,%ecx
    add $0b111,%ecx
	mov %ecx,(%eax)
    add $0x1000,%ebx
    add $0x4,%eax
    cmp %eax, PAGE_TABLE_OFFSET
    je next_page
    jmp fill_table
next_page:
	mov %eax,CURRENT_PAGE
	cmp PAGE_DIR_OFFSET,%edx
	je end_fill_directory
	add $0x4,%edx
	jmp fill_directory
		
end_fill_directory:
	#adjust page dir and page table to allow switch to paging
	mov $PAGE_DIR,%eax
	mov $TMP_PAGE_TABLE,%ebx
      	add $0b111,%ebx
	mov %ebx,(%eax)

	mov $0x0,%ebx
	mov $TMP_PAGE_TABLE,%eax
fill_first_page:
	mov %ebx,%ecx
	add $0b111,%ecx
	mov %ecx,(%eax)
	cmp $0x201000,%ebx
	je end_fill_firt_page
	add $0x1000,%ebx
	add $0x4,%eax
	jmp fill_first_page

end_fill_firt_page:
	mov $PAGE_DIR,%eax;
	add $0xc,%eax;
	mov $TMP_PAGE_TABLE_1,%ebx;
    add $0b111,%ebx;
	mov %ebx,(%eax);

	mov $START_LOADER_PHY_ADD,%ebx
	mov $TMP_PAGE_TABLE_1,%eax
fill_forth_page:
	mov %ebx,%ecx
	add $0b111,%ecx
	mov %ecx,(%eax)
	cmp $END_LOADER_PHY_ADD,%ebx
	je enable_vm
	add $0x1000,%ebx
	add $0x4,%eax
	jmp fill_forth_page
		
# enable virtual memory
enable_vm:
	movl $PAGE_DIR,%eax
	movl %eax,%cr3
	movl %cr0,%eax
	orl $0x80000000,%eax
	movl %eax,%cr0
	movl %ebx,%eax
	movl %ebx,%eax
	jmp init

