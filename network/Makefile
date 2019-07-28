include ../make.rules

vpath %.h $(INCLUDE)/network

all:socket_buffer.o mac.o ip4.o arp.o network.o socket.o tcp.o tcp_socket.o tcp_conn_map.o udp.o icmp.o

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

tcp.o:	tcp.c tcp.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tcp.c

tcp_conn_map.o:	tcp_conn_map.c tcp_conn_map.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tcp_conn_map.c

tcp_socket.o:	tcp_socket.c tcp_socket.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tcp_socket.c

icmp.o:	icmp.c icmp.h
	$(CC) $(CPPFLAGS) $(CFLAGS) icmp.c

clean:	
	rm -f socket_buffer.o mac.o ip4.o arp.o network.o socket.o tcp.o tcp_socket.o tcp_conn_map.o udp.o icmp.o

