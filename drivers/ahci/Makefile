include ../../make.rules

vpath %.h $(INCLUDE)/drivers/ahci

all: 	ahci.o

ahci.o:ahci.c ahci.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) ahci.c

clean:	
	rm -f ahci.o
