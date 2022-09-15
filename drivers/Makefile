include ../make.rules

all:	pit.o ioapic.o lapic.o kbc.o ata.o i8254x.o

pit.o:
	$(MAKE) -C pit
ioapic.o:
	$(MAKE) -C ioapic
lapic.o:
	$(MAKE) -C lapic
kbc.o:
	$(MAKE) -C kbc
ata.o:
	$(MAKE) -C ata
ata.o:
	$(MAKE) -C ahci	
i8254x.o:
	$(MAKE) -C i8254x

clean:
	$(MAKE) -C pit clean
	$(MAKE) -C ioapic clean
	$(MAKE) -C lapic clean
	$(MAKE) -C kbc clean
	$(MAKE) -C ata clean
	$(MAKE) -C ahci clean
	$(MAKE) -C i8254x clean
