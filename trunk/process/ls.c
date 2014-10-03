#include "lib/lib.h"

#define DIR_SIZE 1024
#define READ_BYTE(src,dst) do {unsigned short* p;p=src;dst=(*p)&0xff;} while(0);
#define READ_WORD(src,dst) do {unsigned short* p;p=src;dst=*p;} while(0);
#define READ_DWORD(src,dst) do {unsigned int* p;p=src;dst=*p;} while(0);

int main(int argc, char *argv[])
{
	int f;
	char* io_buffer;
	char file_name[100];
	unsigned int next_entry;
	unsigned int name_len;
	unsigned int rec_len;

	printf("\n");
	printf("argc=");
	print_num(argc);
	printf("\n");
	printf("argv[1]=");
	printf(argv[1]);
	printf("\n");

	io_buffer=malloc(DIR_SIZE);
	
	if (argc>1)
	{
		f=open(argv[1], O_RDWR | O_APPEND);
	}
	else 
	{
		//nedd to propagate from parent to child look current_dir_inode_number inside lookup_inode
		f=open("./", O_RDWR | O_APPEND);
	}
	read(f,io_buffer,DIR_SIZE);
	close(f);

	next_entry=0;
	j=0;
	while(next_entry<=DIR_SIZE)
	{
		READ_BYTE(&io_buffer[next_entry+6],name_len);
		READ_WORD(&io_buffer[next_entry+4],rec_len);
		while(j<name_len)		
		{
			file_name[j]=io_buffer[next_entry+8+j];
			j++;
		}
		next_entry+=rec_len;
		printf(file_name);
	}
	free(DIR_SIZE);
	exit(0);
}
