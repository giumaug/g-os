#include "lib/lib.h"
#include "udpclient.h"

//#include <stdio.h>
//#include <unistd.h>
//#include <stdlib.h>
//#include <string.h>
//#include <netdb.h>
//#include <sys/types.h> 
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/stat.h>
//#include <fcntl.h>

#define PACKET_SIZE 100
#define BUFSIZE 1024
#define IP(a,b,c,d)	(d | (c<<8) | (b<<16) | (a<<24))

typedef int u32;
typedef short u16;

int main(int argc, char **argv) 
{
	unsigned int ip=IP(172,16,6,1);
//	unsigned int ip=IP(172,16,243,1);
	int sockfd,sockfd_1,n_to,i,f,send_len,len;
    	struct sockaddr_in send_addr;
    	char send_buf[BUFSIZE];
        char* io_buffer;
	t_stat stat_data;
	//struct stat stat_data;
	u16 port,fixed_port;
	u32 data_len,len_to_send,offset,sent;

	printf("starting udp_writer \n");

	port=atoi(argv[2]);
	send_addr.sin_family = AF_INET;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[0]=172;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[1]=16;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[2]=6;
	((unsigned char*) &(send_addr.sin_addr.s_addr))[3]=1;
	((unsigned char*) &(send_addr.sin_port))[0]=((unsigned char*) &(port))[1];
	((unsigned char*) &(send_addr.sin_port))[1]=((unsigned char*) &(port))[0];

	stat(argv[1],&stat_data);
	data_len=stat_data.st_size+1;
	io_buffer=malloc(data_len);
	f=open(argv[1], O_RDWR | O_APPEND);
	read(f,io_buffer,data_len);
	close(f);
	io_buffer[data_len]='\0';
	printf(io_buffer);
	free(io_buffer);

	send_len = sizeof(send_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	sent=0;
	offset=-PACKET_SIZE;
	while (!sent)
	{
		offset+=PACKET_SIZE;
		if (data_len-(offset+PACKET_SIZE)>=0)
		{
			len=PACKET_SIZE;
		}
		else
		{
			len=data_len-offset;
			sent=1;
		}
		printf("port is %d \n",port);
		printf("data len is %d \n",data_len);
		printf("1offset is %d \n",offset);
    		n_to = sendto(sockfd,(io_buffer),len,0,&send_addr, send_len);
		printf("2offset is %d \n",offset);
	}
	close(sockfd);
	// kfree ci pensa exit!!!
	free(io_buffer);
    	exit(0);
}
