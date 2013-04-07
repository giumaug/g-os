
include ../make.rules

all:	pic.o pit.o kbc.o ata.o

pic.o:
	$(MAKE) -C pic

pit.o:
	$(MAKE) -C pit

kbc.o:
	$(MAKE) -C kbc
ata.o:
	$(MAKE) -C ata

clean:	
	$(MAKE) -C pit  clean
	$(MAKE) -C pic  clean
	$(MAKE) -C kbc  clean
	$(MAKE) -C ata  clean
