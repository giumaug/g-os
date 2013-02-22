include ../make.rules

vpath %.h $(INCLUDE)/process

#all: 	process_0.o process_1.o process_2.o process_3.o kmalloc_test_proc_1.o kmalloc_test_proc_2.o kmalloc_test_proc_3.o
all: 	process_0.o process_1.o process_2.o process_3.o shell.o

process_0.o:process_0.c process_0.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) process_0.c

process_1.o:process_1.c process_1.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) process_1.c

process_2.o:process_2.c process_2.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) process_2.c

process_3.o:process_3.c process_3.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) process_3.c

process_4.o:process_4.c process_4.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) process_4.c

shell.o:shell.c shell.h
	$(CC)  $(CPPFLAGS) $(CFLAGS) shell.c

clean:	
	rm -f process_0.o process_1.o process_2.o process_3.o shell.o
