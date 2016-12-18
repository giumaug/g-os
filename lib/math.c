#include "lib/lib.h"

inline unsigned int pow2(unsigned int x)
{
	unsigned int ret=1;
	return ret<<x;
}

inline unsigned int log2(unsigned int x)
{
	unsigned int ret;
	asm ("bsf %1,%0;":"=r"(ret):"r"(x));
	return ret;
}

inline u32 min(u32 x,u32 y)
{
	return (((x) < (y)) ? (x) : (y));
}

inline u32 max(u32 x,u32 y)
{
	return (((x) > (y)) ? (x) : (y));
}
