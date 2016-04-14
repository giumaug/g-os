#ifndef LIB_H                
#define LIB_H

#define NULL 0
#define SYSCALL(syscall_num,params) asm("mov %0,%%eax;mov %1,%%ecx;int $0x80"::"r"(syscall_num),"r"(params):"%eax","%ecx");
#define GET_FROM_STACK(n,var) asm("mov %%ebp,%0;":"=r"(var));var+=n+2;

#define O_CREAT 0b1
#define O_APPEND 0b10
#define O_RDWR 0b100

#define RAND_MAX 4294967294

unsigned int seed;

struct s_stat 
{             
	unsigned short st_ino;
	unsigned short st_mode;
	unsigned short st_uid;
	unsigned short st_gid;
	unsigned int st_size;
	unsigned int st_atime;
	unsigned int st_mtime;
	unsigned int st_ctime;
}
typedef t_stat;

#define AF_INET  2

typedef unsigned short  sa_family_t;
typedef unsigned int socklen_t;
typedef int ssize_t;
typedef unsigned int size_t;

struct sockaddr 
{
	sa_family_t     sa_family;
	char            sa_data[6];
};

struct in_addr 
{
	unsigned int   s_addr;
};

struct sockaddr_in 
{
	sa_family_t           sin_family;
	unsigned short int    sin_port;
	struct in_addr        sin_addr; 
};

enum sock_type 
{
	SOCK_STREAM     = 1,
	SOCK_DGRAM      = 2,
	SOCK_RAW        = 3,
	SOCK_RDM        = 4,
	SOCK_SEQPACKET  = 5,
	SOCK_DCCP       = 6,
	SOCK_PACKET     = 10
};

//fcntl.h
int open(const char *fullpath, int flags);

//math.h
unsigned int pow2(unsigned int x);
unsigned int log2(unsigned int x);

//stdio.h
void scanf(char *text,void *val);
int remove(const char *filename);


//stdlib.h
int atoi (char *data);
void itoa (int val,char *char_val,unsigned int base);
void exit(int status);
void *malloc(unsigned int mem_size);
void free(void *address);
//void *bigMalloc(unsigned int mem_size);
//void bigFree(void *address);
unsigned int rand();
int getc();

//string.h
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2,unsigned int n);
unsigned int strlen(const char* s);

//unistd.h
int exec(char* path,char* argv[]);
int fork();
int pause();
int sleep(unsigned int time);
int close(int fd);
int read(int fd, void *buf, int count);
int write(int fd, void *buf, int count);
int chdir(char* path);

//stat.h
int mkdir(const char *fullpath);

//debug.h
void check_free_mem();

//sys/stat.h
int stat(char* pathname, t_stat* stat);

//socket.h
int socket(int socket_family, int socket_type, int protocol);
int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
int recvfrom(int sockfd, void* data,u32 data_len,int flags,struct sockaddr* src_addr,socklen_t* addrlen);
int sendto(int sockfd,void* data,u32  data_len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
int close(int sockfd);

#endif

