#include "lib.h"

int exec(char* path,char* argv[]) 
{
	//make path and argv static!!!!!!
	unsigned int params[3];
	params[0]=path;
	params[1]=argv;
	SYSCALL(14,params);
	return params[2];
}

int fork() 
{
	unsigned int params[2];
	unsigned int ret;
	params[0]=0;//ret val for child
	SYSCALL(1,params);
	if (ret==1000)
	{
		ret=5;
	}
	ret=params[0];//ret val for parent
	//params[1]=0;// ret val for child
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

int close(int fd)
{
	unsigned int params[2];

	params[0]=fd;
	SYSCALL(19,params);
	return  params[1];
}

int read(int fd, void *buf, int count)
{
	unsigned int params[5];

	params[0] = fd;
	params[1] = buf;
	params[2] = count;
	params[3] = 0;
	SYSCALL(109, params);
	return  params[4];
}

int write(int fd, void *buf, int count)
{
	unsigned int params[5];

	params[0] = fd;
	params[1] = buf;
	params[2] = count;
	params[3] = 1;
	SYSCALL(109, params);
	return  params[4];
}

int chdir(char* path)
{
	unsigned int params[1];

	params[0]=path;
	SYSCALL(26,params);
	return  params[1];	
}

int lseek(int fd,int offset,int whence)
{
	unsigned int params[4];

	params[0] = fd;
	params[1] = offset;
	params[2] = whence;	
	SYSCALL(36,params);
	return params[3];	
}

unsigned short getpid()
{
	unsigned int params[1];

	SYSCALL(38,params);
	return params[0];
}

int getpgid(int pid)
{
	unsigned int params[2];

	SYSCALL(39,params);
	return params[1];
}

int setpgid(int pid,int pgid)
{
	unsigned int params[3];

	params[0] = pid;
	params[1] = pgid;
	SYSCALL(40,params);
	return params[2];
}

int tcgetpgrp()
{
	unsigned int params[1];

	SYSCALL(41,params);
	return params[0];
}

int tcsetpgrp(int fg_pgid)
{
	unsigned int params[2];

	params[0] = fg_pgid;
	SYSCALL(42,params);
	return params[1];
}

void read_test()
{
	unsigned int params[1];
	SYSCALL(105,params);

}

void signal()
{
	unsigned int params[1];

	SYSCALL(107,params);
}

void select_dev(int device_num)
{
	unsigned int params[1];

	params[0] = device_num;
	SYSCALL(108,params);	
}
