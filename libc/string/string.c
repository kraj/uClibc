/* vi: set sw=4 ts=4: */
/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 *
 * Many of the functions in this file have been rewritten for correctness
 * (but not necessarily speed) by Erik Andersen <andersee@debian.org>
 */

#include <string.h>
#include <malloc.h>

/********************** Function strlen ************************************/

#ifdef L_strlen
size_t strlen(const char *str)
{
	register char *ptr = (char *) str;

	while (*ptr)
		ptr++;
	return (ptr - str);
}
#endif

/********************** Function strnlen ************************************/

#ifdef L_strnlen
size_t strnlen (const char *string, size_t maxlen)
{
	const char *end = memchr (string, '\0', maxlen);
	return end ? end - string : maxlen;
}
#endif

/********************** Function strcat ************************************/

#ifdef L_strcat
char *strcat(char *dst, const char *src)
{
	strcpy(dst + strlen(dst), src);
	return dst;
}
#endif

/********************** Function strcpy ************************************/

#ifdef L_strcpy
char *strcpy(char *dst, const char *src)
{
	register char *ptr = dst;

	while (*src)
		*dst++ = *src++;
	*dst = '\0';

	return ptr;
}
#endif

/********************** Function stpcpy ************************************/

#ifdef L_stpcpy
char *stpcpy(char *dst, const char *src)
{
	while (*src)
		*dst++ = *src++;
	*dst = '\0';

	return dst;
}
#endif

/********************** Function strcmp ************************************/

#ifdef L_strcmp
int strcmp(const char *s1, const char *s2)
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

#ifndef __UCLIBC_HAS_LOCALE__
weak_alias(strcmp, strcoll);
#endif /* __UCLIBC_HAS_LOCALE__ */
#endif

/***** Function strcoll (locale only, as non-locale is alias of strcmp *****/

#ifdef L_strcoll
#ifdef __UCLIBC_HAS_LOCALE__

#include "../misc/locale/_locale.h"

const unsigned char *_uc_collate_b;  /* NULL for no collate, strcoll->strcmp */

int strcoll(const char *s1, const char *s2)
{
	unsigned char c1, c2;

	while(1) {
		c1 = (unsigned char) *s1;
		c2 = (unsigned char) *s2;
		if(_uc_collate_b) {     /* setuped non-C locale? */
			c1 = _uc_collate_b[c1];
			c2 = _uc_collate_b[c2];
		}
		if (*s1 == '\0' || c1 != c2)
			return c1 - c2;
		s1++;
		s2++;
	}
}
#endif /* __UCLIBC_HAS_LOCALE__ */
#endif

/********************** Function strncat ************************************/

#ifdef L_strncat
char *strncat(char *dst, const char *src, size_t len)
{
	register char *s1 = dst + strlen(dst), *s2;

	s2 = memchr(src, 0, len);
	if (s2) {
		memcpy(s1, src, s2 - src + 1);
	} else {
		memcpy(s1, src, len);
		s1[len] = '\0';
	}
	return dst;
}
#endif

/********************** Function strncpy ************************************/

#ifdef L_strncpy
char *strncpy(char *dst, const char *src, size_t len)
{
	char *ptr = dst;

	while (len--) {
		if (*src)
			*dst++ = *src++;
		else
			*dst++ = '\0';
	}

	return ptr;
}
#endif

#ifdef L_strxfrm
size_t strxfrm(char *dst, const char *src, size_t len)
{
	size_t length;
	strncpy( dst, src, len);
	length = strlen(src);
	if (length<len)
		return(length);
	return( len);
}
#endif	

/********************** Function strncmp ************************************/

#ifdef L_strncmp
int strncmp(const char *s1, const char *s2, size_t len)
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
#endif

/********************** Function strchr ************************************/

#ifdef L_strchr
char *strchr(const char *str, int c)
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

weak_alias(strchr, index);
#endif

/********************** Function strrchr ************************************/

#ifdef L_strrchr

char *strrchr(const char *str, int c)
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

weak_alias(strrchr, rindex);
#endif

/********************** Function strdup ************************************/

#ifdef L_strdup
char *strdup(const char *str)
{
	register size_t len;
	register char *dst;

	len = strlen(str) + 1;
	dst = (char *) malloc(len);
	if (dst)
		memcpy(dst, str, len);
	return dst;
}
#endif

/********************** Function memcpy ************************************/

#ifdef L_memcpy
void *memcpy(void *dst, const void *src, size_t len)
{
	register char *a = dst;
	register const char *b = src;

	while (len--)
		*a++ = *b++;

	return dst;
}
#endif

/********************** Function memccpy ************************************/

#ifdef L_memccpy
void *memccpy(void *dst, const void *src, int c, size_t len)
{
	register char *a = dst;
	register const char *b = src;

	while (len--) {
		if ((*a++ = *b++) == c)
			return a;
	}

	return 0;
}
#endif

/********************** Function memset ************************************/

#ifdef L_memset
void *memset(void *str, int c, size_t len)
{
	register char *a = str;

	while (len--)
		*a++ = c;

	return str;
}
#endif

/********************** Function memmove ************************************/

#ifdef L_memmove
void *memmove(void *dst, const void *src, size_t len)
{
	register char *s1 = dst, *s2 = (char *) src;

	/* This bit of sneakyness c/o Glibc, it assumes the test is unsigned */
	if (s1 - s2 >= len)
		return memcpy(dst, src, len);

	/* This reverse copy only used if we absolutly have to */
	s1 += len;
	s2 += len;
	while (len-- > 0)
		*(--s1) = *(--s2);
	return dst;
}
#endif

/********************** Function memchr ************************************/

#ifdef L_memchr
void *memchr(const void *str, int c, size_t len)
{
	register unsigned char *ptr = (unsigned char *) str;

	while (len--) {
		if (*ptr == (unsigned char) c)
			return ptr;
		ptr++;
	}

	return 0;
}
#endif

/********************** Function memcmp ************************************/

#ifdef L_memcmp
int memcmp(const void *s1, const void *s2, size_t len)
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
#endif


/********************** Function memcmp ************************************/

#ifdef L_ffs
int ffs(int x)
{
	int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}
#endif


/********************** THE END ********************************************/
