#ifndef MATH_HEADER
#define MATH_HEADER
extern inline unsigned int pow2(unsigned int x) __attribute__((always_inline));
extern inline unsigned int log2(unsigned int x) __attribute__((always_inline));


extern inline unsigned int pow2(unsigned int x)
{
	unsigned int ret=1;
	return ret<<x;
}

extern inline unsigned int log2(unsigned int x)
{
	unsigned int ret;
	asm ("bsf %1,%0;":"=r"(ret):"r"(x));
	return ret;
}
#endif

