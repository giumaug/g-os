#/dev/loop7 new image /dev/loop4 iold image
#call with make process=<process_to_compile> all | clean
include ../make.rules

all:	user_space_process.o                      \
        lib.o    

	ld -T linker.ld  -melf_i386 -o $(process) \
	$(process).o                              \
	lib/*.o

install:
	mount /dev/loop7 /mnt
	cp $(BASE_DIR)/process/$(process) /mnt/$(path)
	umount /mnt

user_space_process.o: $(process).c $(process).h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(process).c

lib.o:
	$(MAKE) -C lib

clean:	
	rm -f $(process).o $(process)
	$(MAKE) -C lib clean

