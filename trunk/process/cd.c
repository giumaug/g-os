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

	printf("\n");
	printf("argc=");
	print_num(argc);
	printf("\n");
	printf("argv[1]=");
	printf(argv[1]);
	printf("\n");

	if (argc==2)
	{
		chdir(argv[1]);
	}
	else
	{	
		printf("\n");
		printf("wrong arguments");
		printf("\n");
		
	}
	exit(0);
}
