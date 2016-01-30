ENTRY (main)
process_load_address = 0x100000;

MEMORY { process_address_space : ORIGIN = 0x100000, LENGTH = 0x100000 }

SECTIONS
{
	.text :AT(process_load_address){
	text = .;
	*(.text)
    }>process_address_space

    .rodata :AT(process_load_address + (rodata - text)){
	rodata = .;
        *(.rodata)
    }>process_address_space

    .data :AT(process_load_address + (data - text)){
	data = .;
        *(.data)
    }>process_address_space

    .bss :AT(process_load_address + (bss - text)){
	bss = .;
        *(.bss)
    }>process_address_space
}
