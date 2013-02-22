ENTRY (loader)
kernel_load_address = 0x100000;
process_0_load_address = 0x500000;
process_1_load_address = 0x600000;
process_2_load_address = 0x700000;
process_3_load_address = 0x800000;
process_4_load_address = 0x900000;
process_5_load_address = 0xa00000;

MEMORY { kernel_phy_address_space : ORIGIN = 0xC00000, LENGTH = 0x200000 }
MEMORY { kernel_address_space : ORIGIN = 0xC0000000, LENGTH = 0x200000 }
MEMORY { process_0_address_space : ORIGIN = 0x100000, LENGTH = 0x100000 }
MEMORY { process_1_address_space : ORIGIN = 0x100000, LENGTH = 0x100000 }
MEMORY { process_2_address_space : ORIGIN = 0x100000, LENGTH = 0x100000 }
MEMORY { process_3_address_space : ORIGIN = 0x100000, LENGTH = 0x100000 }
MEMORY { shell_address_space : ORIGIN = 0x100000, LENGTH = 0x100000 }
MEMORY { process_5_address_space : ORIGIN = 0x100000, LENGTH = 0x100000 }

SECTIONS{

    /*kernel_phy_address_space*/

    .text_7 :{
	*loader.o(.text) 
    }>kernel_phy_address_space

    .rodata_7 :{
        *loader.o(.rodata)
    }>kernel_phy_address_space

    .data_7 :{
        *loader.o(.data)
    }>kernel_phy_address_space

    .bss_7 :{
        *loader.o(.bss)
    }>kernel_phy_address_space

    /* kernel address space */ 

    .text :AT(kernel_load_address){
	text = .;
	 *(EXCLUDE_FILE (*process_0.o  *process_1.o *process_2.o *process_3.o *shell.o *process_5.o *loader.o) .text)
    }>kernel_address_space

    .rodata :AT(kernel_load_address + (rodata - text)){
	rodata = .;
	*(EXCLUDE_FILE (*process_0.o  *process_1.o *process_2.o *process_3.o *shell.o *process_5.o *loader.o) .rodata)
    }>kernel_address_space 

    .data :AT(kernel_load_address +(data - text)){
	data = .;
	*(EXCLUDE_FILE (*process_0.o  *process_1.o *process_2.o *process_3.o *shell.o *process_5.o *loader.o) .data)
    }>kernel_address_space 

    .bss :AT(kernel_load_address + (bss - text)){
	bss = .;
	*(EXCLUDE_FILE (*process_0.o  *process_1.o *process_2.o *process_3.o *shell.o *process_5.o *loader.o) .bss)
    }>kernel_address_space

   /*process_0 address space*/ 

    .text_0 :AT(process_0_load_address){
	text_0 = .;
	*process_0.o(.text) 
    }>process_0_address_space

    .rodata_0 :AT(process_0_load_address + (rodata_0 - text_0)){
	rodata_0 = .;
        *process_0.o(.rodata)
    }>process_0_address_space

    .data_0 :AT(process_0_load_address + (data_0 - text_0)){
	data_0 = .;
        *process_0.o(.data)
    }>process_0_address_space

    .bss_0 :AT(process_0_load_address + (bss_0 - text_0)){
	bss_0 = .;
        *process_0.o(.bss)
    }>process_0_address_space

    /*process_1 address space*/ 

    .text_1 :AT(process_1_load_address){
	text_1 = .;
	*process_1.o(.text)
    }>process_1_address_space

    .rodata_1 :AT(process_1_load_address + (rodata_1 - text_1)){
	rodata_1 = .;
        *process_1.o(.rodata)
    }>process_1_address_space

    .data_1 :AT(process_1_load_address + (data_1 - text_1)){
	data_1 = .;
        *process_1.o(.data)
    }>process_1_address_space

    .bss_1 :AT(process_1_load_address + (bss_1 - text_1)){
	bss_1 = .;
        *process_1.o(.bss)
    }>process_1_address_space

    /*process_2 address space*/ 

    .text_2 :AT(process_2_load_address){
	text_2 = .;
	*process_2.o(.text) 
    }>process_2_address_space

    .rodata_2 :AT(process_2_load_address + (rodata_2 - text_2)){
	rodata_2 = .;
        *process_2.o(.rodata)
    }>process_2_address_space

    .data_2 :AT(process_2_load_address + (data_2 - text_2)){
	data_2 = .;
        *process_2.o(.data)
    }>process_2_address_space

    .bss_2 :AT(process_2_load_address + (bss_2 - text_2)){
	bss_2 = .;
        *process_2.o(.bss)
    }>process_2_address_space

    /*process_3 address space*/ 

    .text_3 :AT(process_3_load_address){
	text_3 = .;
	*process_3.o(.text) 
    }>process_3_address_space

    .rodata_3 :AT(process_3_load_address + (rodata_3 - text_3)){
	rodata_3 = .;
        *process_3.o(.rodata)
    }>process_3_address_space

    .data_3 :AT(process_3_load_address + (data_3 - text_3)){
	data_3 = .;
        *process_3.o(.data)
    }>process_3_address_space

    .bss_3 :AT(process_3_load_address + (bss_3 - text_3)){
	bss_3 = .;
        *process_3.o(.bss)
    }>process_3_address_space

    /*shell address space*/

    .text_4 :AT(process_4_load_address){
	text_4 = .;
	*shell.o(.text) 
    }>shell_address_space

    .rodata_4 :AT(process_4_load_address + (rodata_4 - text_4)){
	rodata_4 = .;
        *shell.o(.rodata)
    }>shell_address_space

    .data_4 :AT(process_4_load_address + (data_4 - text_4)){
	data_4 = .;
        *shell.o(.data)
    }>shell_address_space

    .bss_4 :AT(process_4_load_address + (bss_4 - text_4)){
	bss_4 = .;
        *shell.o(.bss)
    }>shell_address_space

    /*process_5  address space*/

    .text_5 :AT(process_5_load_address){
	text_5 = .;
	*process_5.o(.text) 
    }>process_5_address_space

    .rodata_5 :AT(process_5_load_address + (rodata_5 - text_5)){
	rodata_5 = .;
        *process_5.o(.rodata)
    }>process_5_address_space

    .data_5 :AT(process_5_load_address + (data_5 - text_5)){
	data_5 = .;
        *process_5.o(.data)
    }>process_5_address_space

    .bss_5 :AT(process_5_load_address + (bss_5 - text_5)){
	bss_5 = .;
        *process_5.o(.bss)
    }>process_5_address_space
}

