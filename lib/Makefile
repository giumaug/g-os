include ../make.rules

vpath %.h $(INCLUDE)/lib

all:	kmemcpy.o printk.o math.o

kmemcpy.o: kmemcpy.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) kmemcpy.c

printk.o:	printk.c lib.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) printk.c

math.o:	math.c lib.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) math.c

clean:	
	rm -f kmemcpy.o printk.o math.o
