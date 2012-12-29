include ../../make.rules

vpath %.h $(INCLUDE)/drivers/pic

all: 	8259A.o

8259A.o:8259A.c 8259A.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) 8259A.c

clean:	
	rm -f 8259A.o
