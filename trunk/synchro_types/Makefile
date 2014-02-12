include ../make.rules

vpath %.h $(INCLUDE)/synchro_types

all: 	semaphore.o

semaphore.o:semaphore.c semaphore.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) semaphore.c

clean:	
	rm -f semaphore.o
