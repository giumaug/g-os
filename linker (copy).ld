ENTRY (loader)

MEMORY { address_space : ORIGIN = 0x200000, LENGTH = 0x100000 }
/*
MEMORY { address_space_process_1 : ORIGIN = 0x300000, LENGTH = 0x100000 }
MEMORY { address_space_process_2 : ORIGIN = 0x400000, LENGTH = 0x100000 }
MEMORY { address_space_process_3 : ORIGIN = 0x500000, LENGTH = 0x100000 }
*/

SECTIONS{
    . = 0x10000;

     /* kernel address space */

    .text :{
	*(EXCLUDE_FILE (*process.o) .text)
    }

    .rodata : {
	*(EXCLUDE_FILE (*process.o) .rodata)
    }

    .data :{
	*(EXCLUDE_FILE (*process.o) .data)
    }

    .bss :{
	*(EXCLUDE_FILE (*process.o) .bss)
    }

    /*process_0 address space */

    .text_0 :{ 
	process_0.o(.text) 
    }>address_space

    .rodata_0 :{
        process_0.o(.rodata)
    }>address_space

    .data_0 :{
        process_0.o(.data)
    }>address_space

    .bss_0 :{
        process_0.o(.bss)
    }>address_space

/*
    .text_1 :{
        process_1.o(.text)
    }>address_space_process_1

    .rodata_1 :{
        process_1.o(.rodata)
    }>address_space_process_1

    .data_1 :{
        process_1.o(.data)
    }>address_space_process_1

    .bss_1 :{
        process_1.o(.bss)
    }>address_space_process_1


    .text_2 :{
        process_2.o(.text)
    }>address_space_process_2

    .rodata_2 :{
        process_2.o(.rodata)
    }>address_space_process_2

    .data_2 :{
        process_2.o(.data)
    }>address_space_process_2

    .bss_2 :{
        process_2.o(.bss)
    }>address_space_process_2


    .text_3 :{
        process_3.o(.text)
    }>address_space_process_3

    .rodata_3 :{
        process_3.o(.rodata)
    }>address_space_process_3

    .data_3 :{
        process_3.o(.data)
    }>address_space_process_3

    .bss_3 :{
        process_3.o(.bss)
    }>address_space_process_3
*/

}

