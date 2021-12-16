#include "lib/lib.h"  
#include "flush.h"

int main (int argc, char* argv[])
{
	check_free_mem();
	flush_inode_cache();
	check_free_mem();
	exit(0);
}
