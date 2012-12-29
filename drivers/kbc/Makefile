include ../../make.rules

vpath %.h $(INCLUDE)/drivers/kbc

all: 	8042.o

8042.o:8042.c 8042.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) 8042.c

clean:	
	rm -f 8042.o
