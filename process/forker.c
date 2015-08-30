#include "lib/lib.h"  
#include "forker.h"

int main (int argc, char* argv[])
{
	unsigned int pid;

	pid=fork();
	pid=fork();
	pid=fork();
	pid=fork();
	sleep(90000);
	exit(0);
}


