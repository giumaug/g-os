//Linux import
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <sys/stat.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/types.h>

#include "lib/lib.h"

int main(int argc, char* argv[])
{
	unsigned int dir_num;
	unsigned int sub_dir_num;
	unsigned int i,j,z;
	char full_path[100];
    
	char dirs[5][50] = {
				"/aaa1",
				"/aaa2",
				"/aaa3",
				"/aaa4",
				"/aaa5"
	};

	char sub_dirs[5][50] = {
			 	"/ddd1",
				"/ddd1/ddd2",
				"/ddd1/ddd2/ddd3",
				"/ddd1/ddd2/ddd3/ddd4",
				"/ddd1/ddd2/ddd3/ddd4/ddd5"
			   };
//	mkdir("/aaa1");
//	write_file("/aaa1");
//	remove("/aaa1");
//	exit(0);
	for (z = 0; z < 1; z++)
	{
		printf("iteration number %d \n",z);
		mkdir("/aaa1");
		mkdir("/aaa2");
		mkdir("/aaa3");
		mkdir("/aaa4");
		mkdir("/aaa5");
		dir_num = (rand() % 5 + 1);
		if (dir_num > 5)
		{
			printf("overflow on dir_num %d \n",sub_dir_num);
			while(1);
		}
		dir_num = 5;
		//printf("dir_num is %d \n",dir_num);
		for (i = 0; i < dir_num; i++)
		{
			sub_dir_num = (rand() % 5 + 1);
			sub_dir_num = 5;
			//printf("sub_dir_num is %d \n",sub_dir_num);
			write_file(dirs[i]);
			for (j = 0; j < sub_dir_num; j++)
			{
				full_path[0] = '\0';
				strcat(full_path, dirs[i]);
				strcat(full_path, sub_dirs[j]);
				//Linux version.
				//mkdir(full_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				printf("full_path is %s \n",full_path);
				mkdir(full_path);
				write_file(full_path);
			}
		}
		printf("--------------------------\n");
		remove("/aaa1");
		remove("/aaa2");
		remove("/aaa3");
		remove("/aaa4");
		remove("/aaa5");
	}
	exit(0);
}

void write_file(char* src_path)
{
	int i;
	int j;
	int z;
	int file[500];
	int f;
	int src_path_len;
	char** file_names;
	char buffer[] = "aaaaaaaaaa";

	src_path_len = strlen(src_path);
	file_names = malloc(500 * sizeof(char*));
	for ( i = 0 ; i <= 499; i++)
	{
		file_names[i] = malloc(30 + src_path_len);
		strcpy(file_names[i], src_path);
	}

	for (z = 0; z <= 4; z++)
	{
		for (i = 0; i <= 9; i++)
		{
			for (j = 0; j <= 9; j++)
			{
				file_names[100 * z + 10 * i + j][src_path_len] = '/';
				file_names[100 * z + 10 * i + j][src_path_len + 1] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 2] = 'x';

				file_names[100 * z + 10 * i + j][src_path_len + 3] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 4] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 5] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 6] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 7] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 8] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 9] = 'x';
/*
				file_names[100 * z + 10 * i + j][src_path_len + 10] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 11] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 12] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 13] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 14] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 15] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 16] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 17] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 18] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 19] = 'x';
				file_names[100 * z + 10 * i + j][src_path_len + 20] = 'x';
*/
				file_names[100 * z + 10 * i + j][src_path_len + 10] = 48 + z;
				file_names[100 * z + 10 * i + j][src_path_len + 11] = 48 + i;
				file_names[100 * z + 10 * i + j][src_path_len + 12] = 48 + j;
	 			file_names[100 * z + 10 * i + j][src_path_len + 13] = '\0';
			}
		}
	}

//	for (i = 0; i <= 1; i++)
//	{
//		printf("name is %s \n",file_names[i]);
//	}

	for (i = 0; i <= 499; i++)
	{
		f = open(file_names[i], O_CREAT | O_RDWR);
		file[i] = f;
	}

	for (i = 0; i <= 499; i++)
	{
		write(file[i], buffer, 10);
	}
	
	for (i = 0; i <= 499; i++)
	{
		close(file[i]);
	}

	for ( i = 0 ; i <= 499; i++)
	{
		free(file_names[i]);
	}
	free(file_names);
	//flush_inode_cache();
}
