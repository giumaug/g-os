include ../make.rules

vpath %.h $(INCLUDE)/network

all:socket_buffer.o mac.o ip4.o udp.o arp.o network.o socket.o

socket_buffer.o:socket_buffer.c network.h
		$(CC) $(CPPFLAGS) $(CFLAGS) socket_buffer.c

mac.o: 	mac.c mac.h
	$(CC) $(CPPFLAGS) $(CFLAGS) mac.c

ip4.o: ip4.c ip4.h
	$(CC) $(CPPFLAGS) $(CFLAGS) ip4.c

udp.o: udp.c udp.h
	$(CC) $(CPPFLAGS) $(CFLAGS) udp.c

arp.o: 	arp.c arp.h
	$(CC) $(CPPFLAGS) $(CFLAGS) arp.c

network.o: network.c network.h
	$(CC) $(CPPFLAGS) $(CFLAGS) network.c

socket.o:  socket.c socket.h
	$(CC) $(CPPFLAGS) $(CFLAGS) socket.c

clean:	
	rm -f socket_buffer.o mac.o ip4.o udp.o arp.o network.o socket.o

