include make.rules

all:	process.o                      \
        lib.o    

	ld -T linker.ld  -melf_i386 -o $(PROCESS)  \
	*.o                                        \
	lib/*.o   

process.o: $(PROCESS).c $(PROCESS).h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(PROCESS).c

lib.o:
	$(MAKE) -C lib

clean:	
	rm -f $(PROCESS).o
	$(MAKE) -C lib clean
	rm -f $(PROCESS)

