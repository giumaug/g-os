include ../make.rules

vpath %.h $(INCLUDE)/klib

all:	kmemcpy.o printk.o

kmemcpy.o: kmemcpy.c kmemcpy.h
	$(CC) $(CPPFLAGS) $(CFLAGS) kmemcpy.c

printk.o:	printk.c printk.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) printk.c

clean:	
	rm -f kmemcpy.o printk.o
