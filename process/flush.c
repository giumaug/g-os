#include "lib/lib.h"  
#include "flush.h"

int main (int argc, char* argv[])
{
	flush_inode_cache();
	exit(0);
}
