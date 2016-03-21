#include "lib/lib.h"

main()
{
	unsigned int params[2];
	char cmd[100];

	while(1)
	{
		printf("type packet to send \n");
		scanf("%s",&cmd);
		params[0]=cmd;
		params[1]=strlen(cmd);
		SYSCALL(104,params);
	}
	printf("exit........\n");
	exit(0);
}
