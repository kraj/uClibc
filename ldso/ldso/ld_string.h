#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_

#include <sys/types.h>	/* for size_t */

extern void *_dl_malloc(int size);
extern char *_dl_getenv(char *symbol, char **envp);
extern void _dl_unsetenv(char *symbol, char **envp);
extern char *_dl_strdup(const char *string);
extern char *_dl_get_last_path_component(char *path);
extern size_t _dl_strlen(const char * str);
extern char * _dl_strcpy(char * dst,const char *src);
extern int _dl_strcmp(const char * s1,const char * s2);
extern int _dl_strncmp(const char * s1,const char * s2,size_t len);
extern char * _dl_strchr(const char * str,int c);
extern char *_dl_strrchr(const char *str, int c);
extern void * _dl_memcpy(void * dst, const void * src, size_t len);
extern int _dl_memcmp(const void * s1,const void * s2,size_t len);
extern void * _dl_memset(void * str,int c,size_t len);

#ifndef NULL
#define NULL ((void *) 0)
#endif

static inline size_t _dl_strlen_inline(const char * str)
{
	register char *ptr = (char *) str;

	while (*ptr)
		ptr++;
	return (ptr - str);
}

static inline char * _dl_strcpy_inline(char * dst,const char *src)
{
	register char *ptr = dst;

	while (*src)
		*dst++ = *src++;
	*dst = '\0';

	return ptr;
}
 
static inline int _dl_strcmp_inline(const char * s1,const char * s2)
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

static inline int _dl_strncmp_inline(const char * s1,const char * s2,size_t len)
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

static inline char * _dl_strchr_inline(const char * str,int c)
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

static inline char *_dl_strrchr_inline(const char *str, int c)
{
    register char *prev = 0;
    register char *ptr = (char *) str;

    while (*ptr != '\0') {
	if (*ptr == c)
	    prev = ptr;
	ptr++;  
    }   
    if (c == '\0')
	return(ptr);
    return(prev);
}

static inline void * _dl_memcpy_inline(void * dst, const void * src, size_t len)
{
	register char *a = dst;
	register const char *b = src;

	while (len--)
		*a++ = *b++;

	return dst;
}


static inline int _dl_memcmp_inline(const void * s1,const void * s2,size_t len)
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

static inline void * _dl_memset_inline(void * str,int c,size_t len)
{
	register char *a = str;

	while (len--)
		*a++ = c;

	return str;
}

static inline char *_dl_get_last_path_component_inline(char *path)
{
	char *s;
	register char *ptr = path;
	register char *prev = 0;

	while (*ptr)
		ptr++;
	s = ptr - 1;

	/* strip trailing slashes */
	while (s != path && *s == '/') {
		*s-- = '\0';
	}

	/* find last component */
	ptr = path;
	while (*ptr != '\0') {
	    if (*ptr == '/')
		prev = ptr;
	    ptr++;  
	}   
	s = prev;

	if (s == NULL || s[1] == '\0')
		return path;
	else
		return s+1;
}

/* Early on, we can't call printf, so use this to print out
 * numbers using the SEND_STDERR() macro */
static inline char *_dl_simple_ltoa_inline(char * local, unsigned long i)
{
	/* 21 digits plus null terminator, good for 64-bit or smaller ints */
	char *p = &local[21];
	*p-- = '\0';
	do {
		*p-- = '0' + i % 10;
		i /= 10;
	} while (i > 0);
	return p + 1;
}

static inline char *_dl_simple_ltoahex_inline(char * local, unsigned long i)
{
	/* 21 digits plus null terminator, good for 64-bit or smaller ints */
	char *p = &local[21];
	*p-- = '\0';
	do {
		char temp = i % 0x10;
		if (temp <= 0x09)
		    *p-- = '0' + temp;
		else
		    *p-- = 'a' - 0x0a + temp;
		i /= 0x10;
	} while (i > 0);
	*p-- = 'x';
	*p-- = '0';
	return p + 1;
}

#endif
