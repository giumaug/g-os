#include "lib/lib.h"
#include "ping.h"

int main(int argc, char **argv) 
{
	int i,d1,d2,d3,d4,dst_ip;
	int icmp_msg_id;
	int icmp_msg_seq;
	int delay;
	char count;
	char tmp[2];

	icmp_msg_seq = 0;

	tmp[0] = argv[1][0];
	tmp[1] = '\0';
	d1 = atoi(argv[1]);

	tmp[0] = argv[2][0];
	tmp[1] = '\0';
	d2 = atoi(argv[2]);

	tmp[0] = argv[3][0];
	tmp[1] = '\0';
	d3 = atoi(argv[3]);

	tmp[0] = argv[4][0];
	tmp[1] = '\0';
	d4 = atoi(argv[4]);

	tmp[0] = argv[5][0];
	tmp[1] = '\0';
	count = atoi(argv[5]);

	dst_ip = (d1 << 24) | (d2 << 16) | (d3 << 8) | d4;

	for (i = 0;i < count;i++)
	{
		icmp_msg_id = get_pid();
		delay = ping(dst_ip,icmp_msg_id,icmp_msg_seq);
		printf("icmp_seq=%d time=%d ms \n",icmp_msg_seq,delay);
		icmp_msg_seq++;
		sleep(1000);
	}
	exit(0);
}
