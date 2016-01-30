#include "lib/lib.h"

#define DIR_SIZE 1024
#define READ_BYTE(src,dst) do {unsigned short* p;p=src;dst=(*p)&0xff;} while(0);
#define READ_WORD(src,dst) do {unsigned short* p;p=src;dst=*p;} while(0);
#define READ_DWORD(src,dst) do {unsigned int* p;p=src;dst=*p;} while(0);

int main(int argc, char *argv[])
{
	int f;
	int j;
	char* io_buffer;
	char file_name[100];
	unsigned int next_entry;
	unsigned int name_len;
	unsigned int rec_len;
	unsigned int i_number;

//	printf("--argc=%d \n",argc);
//	printf("--argv[0]=%s \n",argv[0]);
//	printf("--argv[1]=%s \n",argv[1]);

	if (argc>1)
	{
		f=open(argv[1], O_RDWR | O_APPEND);
		if (f==-1)
		{
			printf("\n");
			printf("wrong arguments");
			printf("\n");
			exit(0);
		}
	}
	else 
	{
		f=open("./", O_RDWR | O_APPEND);
		if (f==-1)
		{
			printf("\n");
			printf("wrong arguments");
			printf("\n");
			exit(0);
		}
	}
	io_buffer=malloc(DIR_SIZE);
	read(f,io_buffer,DIR_SIZE);
	close(f);

	next_entry=0;
	j=0;
	while(next_entry<DIR_SIZE)
	{
		READ_DWORD(&io_buffer[next_entry],i_number);
		READ_BYTE(&io_buffer[next_entry+6],name_len);
		READ_WORD(&io_buffer[next_entry+4],rec_len);
		
		j=0;
		while(j<name_len)		
		{
			file_name[j]=io_buffer[next_entry+8+j];
			j++;
		}
		next_entry+=rec_len;
		file_name[j]='\0';
		printf(file_name);
		printf("\n");
	}
	free(io_buffer);
	exit(0);
}
