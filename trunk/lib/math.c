unsigned int pow2(unsigned int x)
{
	unsigned int ret=1;
	return ret<<x;
}

unsigned int log2(unsigned int x)
{
	unsigned int ret;
	asm ("bsf %1,%0;":"=r"(ret):"r"(x));
	return ret;
}

