include ../make.rules

vpath %.h $(INCLUDE)/lib

all:	kmemcpy.o printk.o math.o string.o

kmemcpy.o: kmemcpy.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) kmemcpy.c

printk.o:	printk.c lib.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) printk.c

math.o:	math.c lib.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) math.c

string.o: string.c lib.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) string.c

clean:	
	rm -f kmemcpy.o printk.o math.o string.o
