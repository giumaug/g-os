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

typedef unsigned int u32;
typedef unsigned short u16;

int main(int argc, char **argv) 
{
	int sockfd,sockfd_1,n_to,i,f,len,send_len;
    	struct sockaddr_in send_addr;
    	char send_buf[BUFSIZE];
        char* io_buffer;
	t_stat stat_data;
	//struct stat stat_data;
	u16 port,fixed_port;
	u32 len_to_send,sent;
	int data_len,offset;
	char* end="!z!";
	char* pippo="sssssssssssssssssssssssssssssss";

	printf("starting udp_writer... \n");
	unsigned char a1=argv[2][0];
	unsigned char a2=argv[2][1];
	port=(a2<<8)+a1;

	printf("....\n");
	printf("port is.......++... %d \n",port);
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
	printf("data len is %d \n",data_len);
	f=open(argv[1], O_RDWR | O_APPEND);
	//f=open(chicco, O_RDWR | O_APPEND);
	read(f,io_buffer,data_len);
	close(f);
	io_buffer[data_len]='\0';
	//printf(io_buffer);
	//free(io_buffer);

	send_len = sizeof(send_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	sent=1;
	offset=-PACKET_SIZE;
	while (sent!=0)
	{
		offset+=PACKET_SIZE;
		int ff=data_len-(offset+PACKET_SIZE);
		if (data_len-(offset+PACKET_SIZE)>=0)
		{
			len=PACKET_SIZE;
		}
		else
		{
			len=data_len-offset;
			sent=0;
		}
		printf("sending data: %d \n",len);
    		n_to = sendto(sockfd,(io_buffer+offset),len,0,&send_addr, send_len);
		//n_to = sendto(sockfd,(pippo),10,0,&send_addr, send_len);
		printf("sent data \n");
		//printf("offset=%d\n",offset);
		//printf("data_len=%d \n",data_len);
		//printf("ff=%d \n",ff);
		//printf("sent=%d \n",sent);
		//printf("n_to=%d \n",n_to);
	
	}
	//n_to = sendto(sockfd,end,3,0,&send_addr, send_len);
	close(sockfd);
	// kfree ci pensa exit!!!
    	exit(0);
}
