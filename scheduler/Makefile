include ../make.rules

vpath %.h $(INCLUDE)/scheduler

all:	scheduler.o

scheduler.o: scheduler.c scheduler.h
	$(CC) $(CPPFLAGS) $(CFLAGS) scheduler.c
	
clean:	
	rm -f scheduler.o
