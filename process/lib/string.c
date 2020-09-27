#include "lib.h"

int strcmp(const char* s1, const char* s2) 
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

int strncmp(const char* s1, const char* s2,unsigned int n)
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

unsigned int strlen(const char* s)
{
	int i=0;
	while (*s++!='\0') i++;
	return i;
}

char* strstr(char* string, char* substring)	
{
	char* a;
	char* b;

	b = substring;
    	if (*b == 0) 
	{
		return string;
    	}
    	for ( ; *string != 0; string += 1) 
	{
		if (*string != *b) 
		{
	    		continue;
		}
		a = string;
		while (1) 
		{
	    		if (*b == 0) 
			{
				return string;
	    		}
	    		if (*a++ != *b++) 
			{
				break;
	    		}
		}
		b = substring;
    }
    return (char *) 0;
}

char* strcpy(char* dest, const char* src)
{
	char *save = dest;
   	while(*dest++ = *src++);
   	return save;
}

char* strcat(char *dest, const char *src)
{
    size_t i,j;
    for (i = 0; dest[i] != '\0'; i++);
    for (j = 0; src[j] != '\0'; j++)
        dest[i+j] = src[j];
    dest[i + j] = '\0';
    return dest;
}

