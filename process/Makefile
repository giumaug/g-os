include ../make.rules

all:	user_space_process.o                      \
        lib.o    

	ld -T linker.ld  -melf_i386 -o $(PROCESS)  \
	*.o                                        \
	lib/*.o   

user_space_process.o: $(PROCESS).c $(PROCESS).h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(PROCESS).c

lib.o:
	$(MAKE) -C lib

clean:	
	rm -f $(PROCESS).o $(PROCESS)
	$(MAKE) -C lib clean

