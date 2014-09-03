#call with make process=<process_to_compile> all | clean
include ../make.rules

all:	user_space_process.o                      \
        lib.o    

	ld -T linker.ld  -melf_i386 -o $(process)  \
	*.o                                        \
	lib/*.o

install:
	mount /dev/loop3 /mnt
	cp /home/peppe/Desktop/g-os/process/$(process) /mnt/$(path)
	umount /mnt   

user_space_process.o: $(process).c $(process).h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(process).c

lib.o:
	$(MAKE) -C lib

clean:	
	rm -f $(process).o $(process)
	$(MAKE) -C lib clean

