ENTRY (loader)
kernel_load_address = 0x100000;

MEMORY { kernel_phy_address_space : ORIGIN = 0xC00000, LENGTH = 0x200000 }
MEMORY { kernel_address_space : ORIGIN = 0xC0000000, LENGTH = 0x200000 }

SECTIONS{

    /*kernel_phy_address_space*/

    .text_1 :{
	loader.o(.text) 
    }>kernel_phy_address_space

    .rodata_1 :{
        loader.o(.rodata)
    }>kernel_phy_address_space

    .data_1 :{
        loader.o(.data)
    }>kernel_phy_address_space

    .bss_1 :{
        loader.o(.bss)
    }>kernel_phy_address_space

    /* kernel address space */ 

    .text :AT(kernel_load_address){
	text = .;
	 *(EXCLUDE_FILE (loader.o process_0.o).text)
    }>kernel_address_space

    .rodata :AT(kernel_load_address + (rodata - text)){
	rodata = .;
	*(EXCLUDE_FILE (loader.o process_0.o).rodata)
    }>kernel_address_space 

    .data :AT(kernel_load_address +(data - text)){
	data = .;
	*(EXCLUDE_FILE (loader.o process_0.o) .data)
    }>kernel_address_space 

    .bss :AT(kernel_load_address + (bss - text)){
	bss = .;
	*(EXCLUDE_FILE (loader.o process_0.o) .bss)
    }>kernel_address_space
}
