include ../make.rules

vpath %.h $(INCLUDE)/ext2

all:	ext2.o

ext2.o:	ext2.c ext2.h ext2_utils_1.h
	$(CC) $(CPPFLAGS) $(CFLAGS) ext2.c

clean:	
	rm -f ext2.o
