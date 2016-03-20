#include "lib/lib.h"

main()
{
	unsigned int params[1];

	printf("sending test udp packet\n");
	SYSCALL(104,params);
	printf("exit........\n");
	exit(0);
}
