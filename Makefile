include make.rules

vpath %.h $(INCLUDE)

all:	kmain.o                       \
        idt.o                         \
        syscall_handler.o             \
        asm.o loader.o                \
        kernel_init.o                 \
	debug.o                       \
        scheduler.o                   \
        memory_manager.o              \
        virtual_memory.o              \
        console.o klib.o              \
        drivers.o                     \
        data_types.o		      \
	synchro_types.o               \
	ext2.o

	ld -T linker.ld -o kernel.bin \
	*.o                           \
	scheduler/*.o                 \
	memory_manager/*.o            \
	virtual_memory/*.o            \
	console/*.o                   \
	klib/*.o                      \
	drivers/pit/*.o               \
	drivers/pic/*.o               \
	drivers/kbc/*.o               \
	drivers/ata/*.o               \
	data_types/*.o		      \
	synchro_types/*.o             \
	ext2/*.o                

kmain.o:kmain.c
	$(CC) $(CPPFLAGS) $(CFLAGS) kmain.c

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

klib.o:
	$(MAKE) -C klib

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

install_remote:all
	scp ./kernel.bin root@192.168.1.215:/boot/

bochs:all
	mount /dev/loop3 /mnt
	cp /home/peppe/Desktop/g-os/kernel.bin /mnt/boot/kernel.bin
	cp /home/peppe/Desktop/g-os/kernel.bin  /opt/virtutech/simics-3.0.31/workspace/kernel.bin
	umount /mnt
clean:	
	rm -f kmain.o idt.o syscall_handler.o asm.o loader.o kernel_init.o debug.o
	$(MAKE) -C scheduler clean
	$(MAKE) -C memory_manager clean
	$(MAKE) -C virtual_memory clean
	$(MAKE) -C console clean
	$(MAKE) -C klib clean
	$(MAKE) -C drivers clean
	$(MAKE) -C data_types clean
	$(MAKE) -C synchro_types clean
	$(MAKE) -C ext2 clean
	rm -f kernel.bin


