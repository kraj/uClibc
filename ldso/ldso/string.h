#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_

#include <linux/types.h>	/* for size_t */

#ifndef NULL
#define NULL ((void *) 0)
#endif

extern inline char * _dl_strcpy(char * dst,const char *src)
{
	register char *ptr = dst;

	while (*src)
		*dst++ = *src++;
	*dst = '\0';

	return ptr;
}
 
extern inline int _dl_strcmp(const char * s1,const char * s2)
{
	unsigned register char c1, c2;

	do {
		c1 = (unsigned char) *s1++;
		c2 = (unsigned char) *s2++;
		if (c1 == '\0')
			return c1 - c2;
	}
	while (c1 == c2);

	return c1 - c2;
}

extern inline int _dl_strncmp(const char * s1,const char * s2,size_t len)
{
	unsigned register char c1 = '\0';
	unsigned register char c2 = '\0';

	while (len > 0) {
		c1 = (unsigned char) *s1++;
		c2 = (unsigned char) *s2++;
		if (c1 == '\0' || c1 != c2)
			return c1 - c2;
		len--;
	}

	return c1 - c2;
}

extern inline char * _dl_strchr(const char * str,int c)
{
	register char ch;

	do {
		if ((ch = *str) == c)
			return (char *) str;
		str++;
	}
	while (ch);

	return 0;
}


extern inline size_t _dl_strlen(const char * str)
{
	register char *ptr = (char *) str;

	while (*ptr)
		ptr++;
	return (ptr - str);
}

extern inline void * _dl_memcpy(void * dst, const void * src, size_t len)
{
	register char *a = dst;
	register const char *b = src;

	while (len--)
		*a++ = *b++;

	return dst;
}


extern inline int _dl_memcmp(const void * s1,const void * s2,size_t len)
{
	unsigned char *c1 = (unsigned char *)s1;
	unsigned char *c2 = (unsigned char *)s2;

	while (len--) {
		if (*c1 != *c2) 
			return *c1 - *c2;
		c1++;
		c2++;
	}
	return 0;
}

extern inline void * _dl_memset(void * str,int c,size_t len)
{
	register char *a = str;

	while (len--)
		*a++ = c;

	return str;
}

#endif
