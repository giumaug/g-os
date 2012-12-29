include ../make.rules

vpath %.h $(INCLUDE)/console

all:	console.o 

console.o: console.c console.h
	$(CC) $(CPPFLAGS) $(CFLAGS) console.c

clean:	
	rm -f console.o
