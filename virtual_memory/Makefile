include ../make.rules

vpath %.h $(INCLUDE)/virtual_memory

all:	vm.o

vm.o:	vm.c vm.h
	$(CC) $(CPPFLAGS) $(CFLAGS) vm.c

clean:	
	rm -f vm.o
