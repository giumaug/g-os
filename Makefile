#/dev/loop7 new image /dev/loop4 old image
include make.rules

vpath %.h $(INCLUDE)

all:	kmain.o                       \
        timer.o                       \
        idt.o                         \
        syscall_handler.o             \
        asm.o loader.o                \
        kernel_init.o                 \
	debug.o                       \
        scheduler.o                   \
        memory_manager.o              \
        virtual_memory.o              \
        console.o                     \
	lib.o                         \
        drivers.o                     \
        data_types.o		      \
	synchro_types.o               \
	ext2.o                        \
	elf_loader.o                  \
	process_0.o                   \
	mem_regs.o		      \
	network.o	              \
	pci.o

	ld -T linker.ld -melf_i386 -o kernel.bin \
	*.o                           \
	scheduler/*.o                 \
	memory_manager/*.o            \
	virtual_memory/*.o            \
	console/*.o                   \
	lib/*.o                       \
	drivers/pit/*.o               \
	drivers/pic/*.o               \
	drivers/kbc/*.o               \
	drivers/ata/*.o               \
	drivers/i8254x/i8254x.o       \
	data_types/*.o		      \
	synchro_types/*.o             \
	ext2/*.o                      \
	elf_loader/*.o                \
	memory_region/*.o             \
	network/*.o                   \
	pci/*.o    

process_0.o:process_0.c
	$(CC) $(CPPFLAGS) $(CFLAGS) process_0.c

kmain.o:kmain.c
	$(CC) $(CPPFLAGS) $(CFLAGS) kmain.c

timer.o:   timer.c timer.h
	$(CC) $(CPPFLAGS) $(CFLAGS) timer.c

idt.o:	idt.c idt.h
	$(CC) $(CPPFLAGS) $(CFLAGS) idt.c

syscall_handler.o: syscall_handler.c
	$(CC) $(CPPFLAGS) $(CFLAGS) syscall_handler.c

asm.o:	asm.s asm.h
	$(CC) $(CFLAGS) asm.s

loader.o: loader.s
	$(CC) $(CFLAGS) loader.s

kernel_init.o: kernel_init.s
	$(CC) $(CFLAGS) kernel_init.s

debug.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) debug.c

scheduler.o:
	$(MAKE) -C scheduler

memory_manager.o:
	$(MAKE) -C memory_manager

virtual_memory.o:
	$(MAKE) -C virtual_memory

console.o:
	$(MAKE) -C console

lib.o:
	$(MAKE) -C lib

kernel.o:
	$(MAKE) -C kernel

drivers.o:
	$(MAKE) -C drivers

data_types.o:
	$(MAKE) -C data_types

synchro_types.o:
	$(MAKE) -C synchro_types

ext2.o:
	$(MAKE) -C ext2

elf_loader.o:
	$(MAKE) -C elf_loader

mem_regs.o:
	$(MAKE) -C memory_region

pci.o:
	$(MAKE) -C pci

network.o:
	$(MAKE) -C network


install_remote:all
	scp ./kernel.bin root@192.168.1.215:/boot/

bochs:all
	mount /dev/loop7 /mnt
	cp $(BASE_DIR)/kernel.bin /mnt/boot/kernel.bin
	umount /mnt
clean:	
	rm -f kmain.o idt.o syscall_handler.o asm.o loader.o kernel_init.o debug.o process_0.o timer.o
	$(MAKE) -C scheduler clean
	$(MAKE) -C memory_manager clean
	$(MAKE) -C virtual_memory clean
	$(MAKE) -C console clean
	$(MAKE) -C lib clean
	$(MAKE) -C drivers clean
	$(MAKE) -C data_types clean
	$(MAKE) -C synchro_types clean
	$(MAKE) -C ext2 clean
	$(MAKE) -C elf_loader clean
	$(MAKE) -C process clean
	$(MAKE) -C memory_region clean
	$(MAKE) -C pci clean
	$(MAKE) -C network clean
	rm -f kernel.bin


