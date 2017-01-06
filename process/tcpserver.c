#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>

int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(39734);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address,server_len);

	/* Create a connection queue, ignore child exit details and wait forclients. */

	listen(server_sockfd, 5);
	signal(SIGCHLD, SIG_IGN);
	char ch[100];
	while(1) 
	{
		printf("server waiting\n");

		/* Accept connection. */

		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);

		/* Fork to create a process for this client and perform a test to see whether we're the parent or the child. */	
		printf("accepted \n");
		if(fork() == 0) 
		{
			printf("figlio \n");
			/* If we're the child, we can now read/write to the client on client_sockfd. The five second delay is just for this demonstration. */
			int n= read(client_sockfd, ch, 100);
			printf("read = %d \n",n);
			//recv(client_sockfd,&ch,5,0);
			//sleep(5);
			//ch++;
			ch[n]='\0';
			write(client_sockfd, ch, n);
			//send(client_sockfd,ch,n,0);
			printf("sent %s \n",ch);
			printf("..\n");
			close(client_sockfd);
			exit(0);
		}

		/* Otherwise, we must be the parent and our work for this client is finished. */

		else 
		{
			close(client_sockfd);
		}
	}
}
