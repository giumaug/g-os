include ../make.rules

all:	pit.o lapic.o kbc.o ata.o i8254x.o

pit.o:
	$(MAKE) -C pit
lapic.o:
	$(MAKE) -C lapic
kbc.o:
	$(MAKE) -C kbc
ata.o:
	$(MAKE) -C ata

i8254x.o:
	$(MAKE) -C i8254x

clean:
	$(MAKE) -C pit clean
	$(MAKE) -C lapic clean
	$(MAKE) -C kbc clean
	$(MAKE) -C ata clean
	$(MAKE) -C i8254x clean
