#ifndef STRING_HEADER
extern inline int strcmp(const char* s1, const char* s2) __attribute__((always_inline)); 
extern inline int strncmp(const char* s1, const char* cs,unsigned int n) __attribute__((always_inline));
extern inline unsigned int strlen(const char* s) __attribute__((always_inline));

extern inline int strcmp(const char* s1, const char* s2) 
{
	unsigned int c1,c2;
	do
     	{
		c1 = (unsigned char) *s1++;
       		c2 = (unsigned char) *s2++;
       		if (c1 == '\0')
         		return c1 - c2;
     	}
   	while (c1 == c2);
 	return c1 - c2;
}

extern inline int strncmp(const char* s1, const char* s2,unsigned int n)
{
	unsigned int c1,c2;
	while (n > 0)
	{
       		c1 = (unsigned char) *s1++;
       		c2 = (unsigned char) *s2++;
       		if (c1 == '\0' || c1 != c2)
         	return c1 - c2;
       		n--;
     	}
   	return c1 - c2;
}

extern inline unsigned int strlen(const char* s)
{
	int i=0;
	while (*s++!='\0') i++;
	return i;
}
#endif
