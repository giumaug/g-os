include ../../make.rules

#vpath %.h $(INCLUDE)/lib

all.o: math.o stdio.o stdlib.o string.o unistd.o fcntl.o stat.o malloc.o debug.o socket.o

math.o: math.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) math.c

stdio.o: stdio.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) stdio.c

stdlib.o: stdlib.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) stdlib.c

string.o: string.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) string.c

unistd.o: unistd.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) unistd.c

fcntl.o: fcntl.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) fcntl.c

stat.o: stat.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) stat.c

malloc.o: malloc.c malloc.h
	$(CC) $(CPPFLAGS) $(CFLAGS) malloc.c

debug.o: debug.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) debug.c

socket.o: socket.c lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) socket.c

clean:	
	rm -f math.o stdio.o stdlib.o string.o unistd.o fcntl.o stat.o malloc.o debug.o socket.o
