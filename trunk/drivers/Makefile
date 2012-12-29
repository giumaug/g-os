
include ../make.rules

all:	pic.o pit.o kbc.o

pic.o:
	$(MAKE) -C pic

pit.o:
	$(MAKE) -C pit

kbc.o:
	$(MAKE) -C kbc

clean:	
	$(MAKE) -C pit  clean
	$(MAKE) -C pic  clean
	$(MAKE) -C kbc  clean
