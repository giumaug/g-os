include ../make.rules

vpath %.h $(INCLUDE)/pci

all: 	  pci.o

pci.o: pci.c pci.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) pci.c

clean:	
	rm -f pci.o
