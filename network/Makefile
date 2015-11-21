include ../make.rules

vpath %.h $(INCLUDE)/network

all: socket_buffer.o mac.o ip4.o network.o

socket_buffer.o: socket_buffer.c socket_buffer.h
		$(CC) $(CPPFLAGS) $(CFLAGS) socket_buffer.c

mac.o: mac.c mac.h
     $(CC) $(CPPFLAGS) $(CFLAGS) mac.c

ip4.o: ip4.c ip4.h
     $(CC) $(CPPFLAGS) $(CFLAGS) ip4.c

network.o: network.c network.h
     $(CC) $(CPPFLAGS) $(CFLAGS) network.c

clean:	
	rm -f socket_buffer.o mac.o ip4.o network.o

