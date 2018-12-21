include ../make.rules

vpath %.h $(INCLUDE)/data_types

all: 	dllist.o queue.o hashtable.o

dllist.o: dllist.c dllist.h
	$(CC) $(CPPFLAGS) $(CPPFLAGS) $(CFLAGS) dllist.c

queue.o: queue.c queue.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) queue.c

hashtable.o: hashtable.c hashtable.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) hashtable.c

#bit_vector.o: bit_vector.c bit_vector.h
#	$(CC)  $(CPPFLAGS) $(CFLAGS) bit_vector.c
#
clean:	
	rm -f dllist.o queue.o hashtable.o
