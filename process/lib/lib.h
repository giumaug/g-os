#define NULL 0
#define SYSCALL(syscall_num,params) asm("mov %0,%%eax;mov %1,%%ecx;int $0x80"::"r"(syscall_num),"r"(params):"%eax","%ecx");

#define O_CREAT 0b1
#define O_APPEND 0b10
#define O_RDWR 0b100

#define RAND_MAX 4294967294

unsigned int seed;

//fcntl.h
int open(const char *fullpath, int flags);

//math.h
unsigned int pow2(unsigned int x);
unsigned int log2(unsigned int x);

//stdio.h
void print_num(int val);
void d_printf(int val);
void scanf(char *text,void *val);
int remove(const char *filename);

//stdlib.h
int atoi (char *data);
void itoa (int val,char *char_val,unsigned int base);
void exit(int status);
void *malloc(unsigned int mem_size);
void free(void *address);
void *bigMalloc(unsigned int mem_size);
void bigFree(void *address);
unsigned int rand();

//string.h
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2,unsigned int n);
unsigned int strlen(const char* s);

//unistd.h
int exec(unsigned int start_addr,unsigned int size);
int fork();
int pause();
int sleep(unsigned int time);
int close(int fd);
int read(int fd, void *buf, int count);
int write(int fd, void *buf, int count);

//stat.h
int mkdir(const char *fullpath);

