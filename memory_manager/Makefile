include ../make.rules

vpath %.h $(INCLUDE)/memory_manager

all:	mem_proc.o buddy.o fixed_size.o kmalloc.o

mem_proc.o: mem_proc.c mem_proc.h
	$(CC) $(CPPFLAGS) $(CFLAGS) mem_proc.c

buddy.o: buddy.c buddy.h
	$(CC) $(CPPFLAGS) $(CFLAGS) buddy.c

fixed_size.o: fixed_size.c fixed_size.h
	$(CC) $(CPPFLAGS) $(CFLAGS) fixed_size.c

kmalloc.o: kmalloc.c kmalloc.h
	$(CC) $(CPPFLAGS) $(CFLAGS) kmalloc.c

clean:	
	rm -f mem_proc.o buddy.o fixed_size.o kmalloc.o
