include ../../make.rules

vpath %.h $(INCLUDE)/drivers/ioapic/

all: 	ioapic.o

ioapic.o:	ioapic.c ioapic.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) ioapic.c

clean:	
	rm -f ioapic.o
