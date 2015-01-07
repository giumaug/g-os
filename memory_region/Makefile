include ../make.rules

vpath %.h $(INCLUDE)/memory_region

all:	mem_regs.o

mem_regs.o: mem_regs.c mem_regs.h
	$(CC) $(CPPFLAGS) $(CFLAGS) mem_regs.c

clean:	
	rm -f mem_regs.o
