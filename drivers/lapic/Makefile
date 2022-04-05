include ../../make.rules

vpath %.h $(INCLUDE)/drivers/lapic/

all: 	lapic.o

lapic.o:	lapic.c lapic.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) lapic.c

clean:	
	rm -f lapic.o
