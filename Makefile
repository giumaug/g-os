include make.rules

vpath %.h $(INCLUDE)

all:	kmain.o                       \
        idt.o                         \
        syscall_handler.o             \
        asm.o loader.o                \
        kernel_init.o                 \
        scheduler.o                   \
        memory_manager.o              \
        virtual_memory.o              \
        console.o klib.o              \
        drivers.o                     \
        process.o                     \
        data_types.o
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
	process/*.o                   \
	data_types/*.o                

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

process.o:
	$(MAKE) -C process

data_types.o:
	$(MAKE) -C data_types

install_remote:all
	scp ./kernel.bin root@192.168.1.215:/boot/

clean:	
	rm -f kmain.o idt.o syscall_handler.o asm.o loader.o kernel_init.o
	$(MAKE) -C scheduler clean
	$(MAKE) -C memory_manager clean
	$(MAKE) -C virtual_memory clean
	$(MAKE) -C console clean
	$(MAKE) -C klib clean
	$(MAKE) -C drivers clean
	$(MAKE) -C process clean
	$(MAKE) -C data_types clean
	rm -f kernel.bin


