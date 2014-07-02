include ../make.rules

vpath %.h $(INCLUDE)/elf_loader

all:	elf_loader.o

elf_loader.o: elf_loader.c elf_loader.h
	$(CC) $(CPPFLAGS) $(CFLAGS) elf_loader.c

clean:	
	rm -f elf_loader.o
