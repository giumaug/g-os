#include "lib.h"

int exec(char* path,char* argv[]) 
{
	//make path and argv static!!!!!!
	unsigned int params[3];
	params[0]=path;
	params[1]=argv;
	printf("ssss");
	printf(argv[1]);
	SYSCALL(14,params);
	return params[2];
}

int fork() 
{
	unsigned int params[2];
	unsigned int ret;
	params[1]=0;
	params[0]=INIT_VM_USERSPACE;
	SYSCALL(1,params);
	ret=params[1];//ret val for parent
	params[1]=0;// ret val for child
	return ret;
}

int pause()
{
	SYSCALL(11,NULL);
}

int sleep(unsigned int time)
{
	unsigned int params[1];
	params[0]=time;
	SYSCALL(15,params);
}

//extern inline int read_sector(unsigned int lba,unsigned int sector_count,void *read_buffer)
//{
//	unsigned int params[2];
//	params[0]=lba;
//	params[1]=sector_count;
//	params[2]=read_buffer;	
//	SYSCALL(16,params);
//	return params[2];
//}
//
//extern inline int write_sector(unsigned int lba,unsigned int sector_count,void *write_buffer)
//{
//	unsigned int params[2];
//	params[0]=lba;
//	params[1]=sector_count;
//	params[2]=write_buffer;	
//	SYSCALL(17,params);
//	return params[2];
//}

int close(int fd)
{
	unsigned int params[2];

	params[0]=fd;
	SYSCALL(19,params);
	return  params[1];
}

int read(int fd, void *buf, int count)
{
	unsigned int params[4];

	params[0]=fd;
	params[1]=buf;
	params[2]=count;
	SYSCALL(20,params);
	return  params[3];
}

int write(int fd, void *buf, int count)
{
	unsigned int params[4];

	params[0]=fd;
	params[1]=buf;
	params[2]=count;
	SYSCALL(21,params);
	return  params[3];
}

int chdir(char* path)
{
	unsigned int params[1];

	params[0]=path;
	SYSCALL(26,params);
	return  params[1];	
}
