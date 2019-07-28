#ifndef LIB_H                
#define LIB_H

#define NULL 0
#define SYSCALL(syscall_num,params) asm("mov %0,%%eax;mov %1,%%ecx;int $0x80"::"r"(syscall_num),"r"(params):"%eax","%ecx");
#define GET_FROM_STACK(n,var) asm("mov %%ebp,%0;":"=r"(var));var+=n+2;

#define O_CREAT 0b1
#define O_APPEND 0b10
#define O_RDWR 0b100

#define RAND_MAX 4294967294
#define IP(a,b,c,d)	(d | (c<<8) | (b<<16) | (a<<24))

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
#define INADDR_ANY 0

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

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

//fcntl.h
int open(const char *fullpath, int flags);

//math.h
unsigned int pow2(unsigned int x);
unsigned int log2(unsigned int x);

//stdio.h
void scanf(char *text,void *val);
int remove(const char *filename);


//stdlib.h
unsigned int atoi (unsigned char *data);
void itoa (unsigned int val,char *char_val,unsigned int base);
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
char* strstr(char* string, char* substring);

//unistd.h
int exec(char* path,char* argv[]);
int fork();
int pause();
int sleep(unsigned int time);
int close(int fd);
int read(int fd, void *buf, int count);
int write(int fd, void *buf, int count);
int chdir(char* path);
int lseek(int fd,int offset,int whence);
void read_test();
unsigned short getpid();
int getpgid(int pid);
int setpgid(int pid,int pgid);
int tcgetpgrp();
int tcsetpgrp(int fg_pgid);

//stat.h
int mkdir(const char *fullpath);

//debug.h
void check_free_mem();

//sys/stat.h
int stat(char* pathname, t_stat* stat);

//socket.h
int socket(int socket_family, int socket_type, int protocol);
int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
int recvfrom(int sockfd, void* data,size_t data_len,int flags,struct sockaddr* src_addr,socklen_t* addrlen);
int sendto(int sockfd,void* data,size_t  data_len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
int connect(int sockfd, const struct sockaddr *address,socklen_t len);
int write_socket(int sockfd, void *buf, int count);
int read_socket(int sockfd, void *buf, int count);
int close_socket(int sockfd);
int ping(unsigned int dst_ip,unsigned short icmp_msg_id,unsigned short icmp_msg_seq);


#endif

