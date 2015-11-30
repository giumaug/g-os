include ../make.rules

vpath %.h $(INCLUDE)/network

all:socket_buffer.o mac.o ip4.o arp.o network.o

socket_buffer.o:socket_buffer.c network.h
		$(CC) $(CPPFLAGS) $(CFLAGS) socket_buffer.c

mac.o: 	mac.c network.h
	$(CC) $(CPPFLAGS) $(CFLAGS) mac.c

ip4.o: ip4.c network.h
	$(CC) $(CPPFLAGS) $(CFLAGS) ip4.c

arp.o: 	arp.c network.h
	$(CC) $(CPPFLAGS) $(CFLAGS) arp.c

network.o: network.c network.h
	$(CC) $(CPPFLAGS) $(CFLAGS) network.c

clean:	
	rm -f socket_buffer.o mac.o ip4.o arp.o network.o

