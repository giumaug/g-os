include ../../make.rules

vpath %.h $(INCLUDE)/drivers/i8254x

all: 	  i8254x.o

i8254x.o: i8254x.c i8254x.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) i8254x.c

clean:	
	rm -f i8254x.o
