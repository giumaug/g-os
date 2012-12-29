include ../../make.rules

vpath %.h $(INCLUDE)/drivers/pit/

all: 	8253.o

8253.o:	8253.c 8253.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) 8253.c

clean:	
	rm -f 8253.o
