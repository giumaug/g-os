include ../make.rules

vpath %.h $(INCLUDE)/framebuffer

all:	framebuffer.o 

framebuffer.o: framebuffer.c framebuffer.h
	$(CC) $(CPPFLAGS) $(CFLAGS) framebuffer.c

clean:	
	rm -f framebuffer.o
