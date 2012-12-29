include ../make.rules

vpath %.h $(INCLUDE)/data_types

all: 	dllist.o queue.o

dllist.o: dllist.c dllist.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) dllist.c

queue.o: queue.c queue.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) queue.c

clean:	
	rm -f dllist.o queue.o
