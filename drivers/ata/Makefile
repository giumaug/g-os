include ../../make.rules

vpath %.h $(INCLUDE)/drivers/ata

all: 	ata.o

ata.o:ata.c ata.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) ata.c

clean:	
	rm -f ata.o
