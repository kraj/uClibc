/*  Copyright (C) 2002     Manuel Novoa III
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*  ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!
 *
 *  Besides uClibc, I'm using this code in my libc for elks, which is
 *  a 16-bit environment with a fairly limited compiler.  It would make
 *  things much easier for me if this file isn't modified unnecessarily.
 *  In particular, please put any new or replacement functions somewhere
 *  else, and modify the makefile to use your version instead.
 *  Thanks.  Manuel
 *
 *  ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION! */

#define _GNU_SOURCE
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef WANT_WIDE
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

#define Wvoid			wchar_t
#define Wchar			wchar_t
#define Wuchar			__uwchar_t
#define Wint			wchar_t

#else

#define Wvoid			void
#define Wchar			char
typedef unsigned char	__string_uchar_t;
#define Wuchar			__string_uchar_t
#define Wint			int

#endif

/**********************************************************************/
#ifdef L_wmemcpy
#define L_memcpy
#define Wmemcpy wmemcpy
#else
#define Wmemcpy memcpy
#endif

#ifdef L_memcpy

Wvoid *Wmemcpy(Wvoid * __restrict s1, const Wvoid * __restrict s2, size_t n)
{
	register Wchar *r1 = s1;
	register const Wchar *r2 = s2;

#ifdef __BCC__
	while (n--) {
		*r1++ = *r2++;
	}
#else
	while (n) {
		*r1++ = *r2++;
		--n;
	}
#endif

	return s1;
}

#endif
/**********************************************************************/
#ifdef L_wmemmove
#define L_memmove
#define Wmemmove wmemmove
#else
#define Wmemmove memmove
#endif

#ifdef L_memmove

Wvoid *Wmemmove(Wvoid *s1, const Wvoid *s2, size_t n)
{
#ifdef __BCC__
	register Wchar *s = (Wchar *) s1;
	register const Wchar *p = (const Wchar *) s2;

	if (p >= s) {
		while (n--) {
			*s++ = *p++;
		}
	} else {
		s += n;
		p += n;
		while (n--) {
			*--s = *--p;
		}
	}

	return s1;
#else
	register Wchar *s = (Wchar *) s1;
	register const Wchar *p = (const Wchar *) s2;

	if (p >= s) {
		while (n) {
			*s++ = *p++;
			--n;
		}
	} else {
		while (n) {
			--n;
			s[n] = p[n];
		}
	}

	return s1;
#endif
}

#endif
/**********************************************************************/
#ifdef L_wcscpy
#define L_strcpy
#define Wstrcpy wcscpy
#else
#define Wstrcpy strcpy
#endif

#ifdef L_strcpy

Wchar *Wstrcpy(Wchar * __restrict s1, const Wchar * __restrict s2)
{
	register Wchar *s = s1;

#ifdef __BCC__
	do {
		*s = *s2++;
	} while (*s++ != 0);
#else
	while ( (*s++ = *s2++) != 0 );
#endif

	return s1;
}

#endif
/**********************************************************************/
#ifdef L_wcsncpy
#define L_strncpy
#define Wstrncpy wcsncpy
#else
#define Wstrncpy strncpy
#endif

#ifdef L_strncpy

Wchar *Wstrncpy(Wchar * __restrict s1, register const Wchar * __restrict s2,
				size_t n)
{
	register Wchar *s = s1;

#ifdef __BCC__
	while (n--) {
		if ((*s = *s2) != 0) s2++; /* Need to fill tail with 0s. */
		++s;
	}
#else
	while (n) {
		if ((*s = *s2) != 0) s2++; /* Need to fill tail with 0s. */
		++s;
		--n;
	}
#endif
	
	return s1;
}

#endif
/**********************************************************************/
#ifdef L_wcscat
#define L_strcat
#define Wstrcat wcscat
#else
#define Wstrcat strcat
#endif

#ifdef L_strcat

Wchar *Wstrcat(Wchar * __restrict s1, register const Wchar * __restrict s2)
{
	register Wchar *s = s1;

	while (*s++);
	--s;
	while ((*s++ = *s2++) != 0);

	return s1;
}

#endif
/**********************************************************************/
#ifdef L_wcsncat
#define L_strncat
#define Wstrncat wcsncat
#else
#define Wstrncat strncat
#endif

#ifdef L_strncat

Wchar *Wstrncat(Wchar * __restrict s1, register const Wchar * __restrict s2,
				size_t n)
{
	register Wchar *s = s1;

	while (*s++);
	--s;
#if __BCC__
	while (n-- && ((*s = *s2++) != 0)) ++s;
#else
	while (n && ((*s = *s2++) != 0)) {
		--n;
		++s;
	}
#endif
	*s = 0;

	return s1;
}

#endif
/**********************************************************************/
#ifdef L_wmemcmp
#define L_memcmp
#define Wmemcmp wmemcmp
#else
#define Wmemcmp memcmp
#endif

#ifdef L_memcmp

#ifndef L_wmemcmp
weak_alias(memcmp,bcmp)
#endif

int Wmemcmp(const Wvoid *s1, const Wvoid *s2, size_t n)
{
	register const Wuchar *r1 = (const Wuchar *) s1;
	register const Wuchar *r2 = (const Wuchar *) s2;

#ifdef WANT_WIDE
	while (n && (*r1 == *r2)) {
		++r1;
		++r2;
		--n;
	}

	return (n == 0) ? 0 : ((*r1 < *r2) ? -1 : 1);
#else
	int r = 0;

	while (n-- && ((r = ((int)(*r1++)) - *r2++) == 0));

	return r;
#endif
}

#endif
/**********************************************************************/
#ifdef L_wcscmp
#define L_strcmp
#define Wstrcmp wcscmp
#else
#define Wstrcmp strcmp
#endif

#ifdef L_strcmp

#ifndef L_wcscmp
#warning implement strcoll and remove weak alias (or enable for C locale only)
weak_alias(strcmp, strcoll);
#endif

int Wstrcmp(register const Wchar *s1, register const Wchar *s2)
{
#ifdef WANT_WIDE
	while (*((Wuchar *)s1) == *((Wuchar *)s2)) {
		if (!*s1++) {
			return 0;
		}
		++s2;
	}

	return (*((Wuchar *)s1) < *((Wuchar *)s2)) ? -1 : 1;
#else
	int r;

	while (((r = ((int)(*((Wuchar *)s1))) - *((Wuchar *)s2++))
			== 0) && *s1++);
	
	return r;
#endif
}
#endif
/**********************************************************************/
#ifdef L_strcoll
#error implement strcoll and remove weak_alias!!

#if 0
extern unsigned char *_ctype_collate;
int strcoll(register const char *s1, const char *s2)
{
    int r;

    while (!(r = (_ctype_collate[(int)(*s1++)]-_ctype_collate[(int)(*s2++)])));

    return r;
}
#endif

#endif
/**********************************************************************/
#ifdef L_wcsncmp
#define L_strncmp
#define Wstrncmp wcsncmp
#else
#define Wstrncmp strncmp
#endif

#ifdef L_strncmp

int Wstrncmp(register const Wchar *s1, register const Wchar *s2, size_t n)
{
#ifdef WANT_WIDE
	while (n && (*((Wuchar *)s1) == *((Wuchar *)s2))) {
		if (!*s1++) {
			return 0;
		}
		++s2;
		--n;
	}

	return (n == 0) ? 0 : ((*((Wuchar *)s1) < *((Wuchar *)s2)) ? -1 : 1);
#else
	int r = 0;

	while (n--
		   && ((r = ((int)(*((unsigned char *)s1))) - *((unsigned char *)s2++))
			== 0)
		   && *s1++);

	return r;
#endif
}

#endif
/**********************************************************************/
#ifdef L_strxfrm
#error implement strxfrm
/* size_t strxfrm(char *dst, const char *src, size_t len); */
#endif	
/**********************************************************************/
#ifdef L_wmemchr
#define L_memchr
#define Wmemchr wmemchr
#else
#define Wmemchr memchr
#endif

#ifdef L_memchr

Wvoid *Wmemchr(const Wvoid *s, Wint c, size_t n)
{
	register const Wuchar *r = (const Wuchar *) s;
#ifdef __BCC__
	/* bcc can optimize the counter if it thinks it is a pointer... */
	register const char *np = (const char *) n;
#else
#define np n
#endif

	while (np) {
		if (*r == ((Wuchar)c)) {
			return (Wvoid *) r;	/* silence the warning */
		}
		++r;
		--np;
	}

	return NULL;
}
#undef np

#endif
/**********************************************************************/
#ifdef L_wcschr
#define L_strchr
#define Wstrchr wcschr
#else
#define Wstrchr strchr
#endif

#ifdef L_strchr

#ifndef L_wcschr
weak_alias(strchr,index)
#endif

Wchar *Wstrchr(register const Wchar *s, Wint c)
{
	do {
		if (*s == ((Wchar)c)) {
			return (Wchar *) s;	/* silence the warning */
		}
	} while (*s++);

	return NULL;
}

#endif
/**********************************************************************/
#ifdef L_wcscspn
#define L_strcspn
#define Wstrcspn wcscspn
#else
#define Wstrcspn strcspn
#endif

#ifdef L_strcspn

size_t Wstrcspn(const Wchar *s1, const Wchar *s2)
{
	register const Wchar *s;
	register const Wchar *p;

	for ( s=s1 ; *s ; s++ ) {
		for ( p=s2 ; *p ; p++ ) {
			if (*p == *s) goto done;
		}
	}
 done:
	return s - s1;
}

#endif
/**********************************************************************/
#ifdef L_wcspbrk
#define L_strpbrk
#define Wstrpbrk wcspbrk
#else
#define Wstrpbrk strpbrk
#endif

#ifdef L_strpbrk

Wchar *Wstrpbrk(const Wchar *s1, const Wchar *s2)
{
	register const Wchar *s;
	register const Wchar *p;

	for ( s=s1 ; *s ; s++ ) {
		for ( p=s2 ; *p ; p++ ) {
			if (*p == *s) return (Wchar *) s; /* silence the warning */
		}
	}
	return NULL;
}
#endif
/**********************************************************************/
#ifdef L_wcsrchr
#define L_strrchr
#define Wstrrchr wcsrchr
#else
#define Wstrrchr strrchr
#endif

#ifdef L_strrchr

#ifndef L_wcsrchr
weak_alias(strrchr,rindex)
#endif

Wchar *Wstrrchr(register const  Wchar *s, Wint c)
{
	register const Wchar *p;

	p = NULL;
	do {
		if (*s == (Wchar) c) {
			p = s;
		}
	} while (*s++);

	return (Wchar *) p;			/* silence the warning */
}

#endif
/**********************************************************************/
#ifdef L_wcsspn
#define L_strspn
#define Wstrspn wcsspn
#else
#define Wstrspn strspn
#endif

#ifdef L_strspn

size_t Wstrspn(const Wchar *s1, const Wchar *s2)
{
	register const Wchar *s = s1;
	register const Wchar *p = s2;

	while (*p) {
		if (*p++ == *s) {
			++s;
			p = s2;
		}
	}
	return s - s1;
}

#endif
/**********************************************************************/
#ifdef L_wcsstr
#define L_strstr
#define Wstrstr wcsstr
#else
#define Wstrstr strstr
#endif

#ifdef L_strstr

/* NOTE: This is the simple-minded O(len(s1) * len(s2)) worst-case approach. */

#ifdef L_wcsstr
weak_alias(wcsstr,wcswcs)
#endif

Wchar *Wstrstr(const Wchar *s1, const Wchar *s2)
{
	register const Wchar *s = s1;
	register const Wchar *p = s2;

	do {
		if (!*p) {
			return (Wchar *) s1;;
		}
		if (*p == *s) {
			++p;
			++s;
		} else {
			p = s2;
			if (!*s) {
				return NULL;
			}
			s = ++s1;
		}
	} while (1);
}

#endif
/**********************************************************************/
#ifdef L_wcstok
#define L_strtok_r
#define Wstrtok_r wcstok
#define Wstrspn wcsspn
#define Wstrpbrk wcspbrk
#else
#define Wstrtok_r strtok_r
#define Wstrspn strspn
#define Wstrpbrk strpbrk
#endif

#ifdef L_strtok_r

Wchar *Wstrtok_r(Wchar * __restrict s1, const Wchar * __restrict s2,
				 Wchar ** __restrict next_start)
{
	register Wchar *s;
	register Wchar *p;

#if 1
	if (((s = s1) != NULL) || ((s = *next_start) != NULL)) {
		if (*(s += Wstrspn(s, s2))) {
			if ((p = Wstrpbrk(s, s2)) != NULL) {
				*p++ = 0;
			}
		} else {
			p = s = NULL;
		}
		*next_start = p;
	}
	return s;
#else
	if (!(s = s1)) {
		s = *next_start;
	}
	if (s && *(s += Wstrspn(s, s2))) {
		if (*(p = s + Wstrcspn(s, s2))) {
			*p++ = 0;
		}
		*next_start = p;
		return s;
	}
	return NULL;				/* TODO: set *next_start = NULL for safety? */
#endif
}

#endif
/**********************************************************************/
/*  #ifdef L_wcstok */
/*  #define L_strtok */
/*  #define Wstrtok wcstok */
/*  #define Wstrtok_r wcstok_r */
/*  #else */
/*  #define Wstrtok strtok */
/*  #define Wstrtok_r strtok_r */
/*  #endif */

#ifdef L_strtok
#define Wstrtok strtok
#define Wstrtok_r strtok_r

Wchar *Wstrtok(Wchar * __restrict s1, const Wchar * __restrict s2)
{
	static Wchar *next_start;	/* Initialized to 0 since in bss. */
	return Wstrtok_r(s1, s2, &next_start);
}

#endif
/**********************************************************************/
#ifdef L_wmemset
#define L_memset
#define Wmemset wmemset
#else
#define Wmemset memset
#endif

#ifdef L_memset

Wvoid *Wmemset(Wvoid *s, Wint c, size_t n)
{
	register Wuchar *p = (Wuchar *) s;
#ifdef __BCC__
	/* bcc can optimize the counter if it thinks it is a pointer... */
	register const char *np = (const char *) n;
#else
#define np n
#endif

	while (np) {
		*p++ = (Wuchar) c;
		--np;
	}

	return s;
}
#undef np

#endif
/**********************************************************************/
#ifdef L_strerror
#error implement strerror
/*  char *strerror(int errnum); */
#endif
/**********************************************************************/
#ifdef L_wcslen
#define L_strlen
#define Wstrlen wcslen
#else
#define Wstrlen strlen
#endif

#ifdef L_strlen

size_t Wstrlen(const Wchar *s)
{
	register const Wchar *p;

	for (p=s ; *p ; p++);

	return p - s;
}

#endif
/**********************************************************************/
/* ANSI/ISO end here */
/**********************************************************************/
#ifdef L_ffs

int ffs(int i)
{
#if 1
	/* inlined binary search method */
	char n = 1;
#if UINT_MAX == 0xffffU
	/* nothing to do here -- just trying to avoiding possible problems */
#elif UINT_MAX == 0xffffffffU
	if (!(i & 0xffff)) {
		n += 16;
		i >>= 16;
	}
#else
#error ffs needs rewriting!
#endif

	if (!(i & 0xff)) {
		n += 8;
		i >>= 8;
	}
	if (!(i & 0x0f)) {
		n += 4;
		i >>= 4;
	}
	if (!(i & 0x03)) {
		n += 2;
		i >>= 2;
	}
	return (i) ? (n + ((i+1) & 0x01)) : 0;

#else
	/* linear search -- slow, but small */
	int n;

	for (n = 0 ; i ; ++n) {
		i >>= 1;
	}
	
	return n;
#endif
}

#endif
/**********************************************************************/
#ifdef L_wcscasecmp
#define L_strcasecmp
#define Wstrcasecmp wcscasecmp
#else
#define Wstrcasecmp strcasecmp
#endif

#ifdef L_strcasecmp

int Wstrcasecmp(register const Wchar *s1, register const Wchar *s2)
{
#ifdef WANT_WIDE
	while ((*s1 == *s2) || (towlower(*s1) == towlower(*s2))) {
		if (!*s1++) {
			return 0;
		}
		++s2;
	}

	return (((Wuchar)towlower(*s1)) < ((Wuchar)towlower(*s2))) ? -1 : 1;
	/* TODO -- should wide cmp funcs do wchar or Wuchar compares? */
#else
	int r = 0;

	while ( ((s1 == s2) ||
			 !(r = ((int)( tolower(*((Wuchar *)s1))))
			   - tolower(*((Wuchar *)s2))))
			&& (++s2, *s1++));

	return r;
#endif
}

#endif
/**********************************************************************/
#ifdef L_wcsncasecmp
#define L_strncasecmp
#define Wstrncasecmp wcsncasecmp
#else
#define Wstrncasecmp strncasecmp
#endif

#ifdef L_strncasecmp

int Wstrncasecmp(register const Wchar *s1, register const Wchar *s2, size_t n)
{
#ifdef WANT_WIDE
	while (n && ((*s1 == *s2) || (towlower(*s1) == towlower(*s2)))) {
		if (!*s1++) {
			return 0;
		}
		++s2;
		--n;
	}

	return (n == 0)
		? 0
		: ((((Wuchar)towlower(*s1)) < ((Wuchar)towlower(*s2))) ? -1 : 1);
	/* TODO -- should wide cmp funcs do wchar or Wuchar compares? */
#else
	int r = 0;

	while ( n
			&& ((s1 == s2) ||
				!(r = ((int)( tolower(*((unsigned char *)s1))))
				  - tolower(*((unsigned char *)s2))))
			&& (--n, ++s2, *s1++));
	return r;
#endif
}
#endif
/**********************************************************************/
#ifdef L_wcsnlen
#define L_strnlen
#define Wstrnlen wcsnlen
#else
#define Wstrnlen strnlen
#endif

#ifdef L_strnlen

size_t Wstrnlen(const Wchar *s, size_t max)
{
	register const Wchar *p = s;
#ifdef __BCC__
	/* bcc can optimize the counter if it thinks it is a pointer... */
	register const char *maxp = (const char *) max;
#else
#define maxp max
#endif

	while (maxp && *p) {
		++p;
		--maxp;
	}

	return p - s;
}
#undef maxp
#endif
/**********************************************************************/
/* No wide analog. */

#ifdef L_memccpy

void *memccpy(void * __restrict s1, const void * __restrict s2, int c, size_t n)
{
	register char *r1 = s1;
	register const char *r2 = s2;

	while (n-- && (((unsigned char)(*r1++ = *r2++)) != ((unsigned char) c)));

	return (n == (size_t) -1) ? NULL : r1;
}

#endif
/**********************************************************************/
#ifdef L_wcsdup
#define L_strdup
#define Wstrdup wcsdup
#define Wstrlen wcslen
#define Wstrcpy wcscpy
#else
#define Wstrdup strdup
#define Wstrlen strlen
#define Wstrcpy strcpy
#endif

#ifdef L_strdup

Wchar *Wstrdup(register const Wchar *s1)
{
	register Wchar *s;

    if ((s = malloc((Wstrlen(s1) + 1) * sizeof(Wchar))) != NULL) {
		Wstrcpy(s, s1);
	}

	return s;
}

#endif
/**********************************************************************/
/* GNU extension functions. */
/**********************************************************************/
#ifdef L_wmempcpy
#define L_mempcpy
#define Wmempcpy wmempcpy
#else
#define Wmempcpy mempcpy
#endif

#ifdef L_mempcpy

#ifndef L_wmempcpy
/* uClibc's old string implementation did this to cater to some app. */
weak_alias(mempcpy,__mempcpy)
#endif

Wvoid *Wmempcpy(Wvoid * __restrict s1, const Wvoid * __restrict s2, size_t n)
{
	register Wchar *r1 = s1;
	register const Wchar *r2 = s2;

#ifdef __BCC__
	while (n--) {
		*r1++ = *r2++;
	}
#else
	while (n) {
		*r1++ = *r2++;
		--n;
	}
#endif

	return r1;
}

#endif
/**********************************************************************/
#ifdef L_memrchr

void *memrchr(const void *s, int c, size_t n)
{
	register const unsigned char *r;
#ifdef __BCC__
	/* bcc can optimize the counter if it thinks it is a pointer... */
	register const char *np = (const char *) n;
#else
#define np n
#endif
	
	r = ((unsigned char *)s) + ((size_t) np);

	while (np) {
		if (*--r == ((unsigned char)c)) {
			return (void *) r;	/* silence the warning */
		}
		--np;
	}

	return NULL;
}
#undef np

#endif
/**********************************************************************/
#ifdef L_stpcpy

char *stpcpy(register char * __restrict s1, const char * __restrict s2)
{
#ifdef __BCC__
	do {
		*s1 = *s2++;
	} while (*s1++ != 0);
#else
	while ( (*s1++ = *s2++) != 0 );
#endif

	return s1 - 1;
}

#endif
/**********************************************************************/
#ifdef L_stpncpy

char *stpncpy(register char * __restrict s1,
			  register const char * __restrict s2,
			  size_t n)
{
	char *s = s1;
	const char *p = s2;

#ifdef __BCC__
	while (n--) {
		if ((*s = *s2) != 0) s2++; /* Need to fill tail with 0s. */
		++s;
	}
	return s1 + (s2 - p);
#else
	while (n) {
		if ((*s = *s2) != 0) s2++; /* Need to fill tail with 0s. */
		++s;
		--n;
	}
	return s1 + (s2 - p);
#endif
}

#endif
/**********************************************************************/
#ifdef L_bzero

void bzero(void *s, size_t n)
{
	register unsigned char *p = s;
#ifdef __BCC__
	/* bcc can optimize the counter if it thinks it is a pointer... */
	register const char *np = (const char *) n;
#else
#define np n
#endif

	while (np) {
		*p++ = 0;
		--np;
	}
}
#undef np

#endif
/**********************************************************************/
#ifdef L_bcopy

void bcopy(const void *s2, void *s1, size_t n)
{
#if 1
	memmove(s1, s2, n);
#else
#ifdef __BCC__
	register char *s;
	register const char *p;

	s = s1;
	p = s2;
	if (p >= s) {
		while (n--) {
			*s++ = *p++;
		}
	} else {
		s += n;
		p += n;
		while (n--) {
			*--s = *--p;
		}
	}
#else
	register char *s;
	register const char *p;

	s = s1;
	p = s2;
	if (p >= s) {
		while (n) {
			*s++ = *p++;
			--n;
		}
	} else {
		while (n) {
			--n;
			s[n] = p[n];
		}
	}
#endif
#endif
}

#endif
/**********************************************************************/
#ifdef L_strcasestr

char *strcasestr(const char *s1, const char *s2)
{
	register const char *s = s1;
	register const char *p = s2;

#if 1
	do {
		if (!*p) {
			return (char *) s1;;
		}
		if ((*p == *s)
			|| (tolower(*((unsigned char *)p)) == tolower(*((unsigned char *)s)))
			) {
			++p;
			++s;
		} else {
			p = s2;
			if (!*s) {
				return NULL;
			}
			s = ++s1;
		}
	} while (1);
#else
	while (*p && *s) {
		if ((*p == *s)
			|| (tolower(*((unsigned char *)p)) == tolower(*((unsigned char *)s)))
			) {
			++p;
			++s;
		} else {
			p = s2;
			s = ++s1;
		}
	}

	return (*p) ? NULL : (char *) s1;
#endif
}

#endif
/**********************************************************************/
#ifdef L_strndup

char *strndup(register const char *s1, size_t n)
{
	register char *s;

	n = strnlen(s1,n);			/* Avoid problems if s1 not nul-terminated. */

    if ((s = malloc(n + 1)) != NULL) {
		memcpy(s, s1, n);
		s[n] = 0;
	}

	return s;
}

#endif
/**********************************************************************/
#ifdef L_strsep

char *strsep(char ** __restrict s1, const char * __restrict s2)
{
	register char *s = *s1;
	register char *p;

#if 1
	p = NULL;
	if (s && *s && (p = strpbrk(s, s2))) {
		*p++ = 0;
	}
#else
	if (s && *s && *(p = s + strcspn(s, s2))) {
		*p++ = 0;
	} else {
		p = NULL;
	}
#endif
	*s1 = p;
	return s;
}

#endif
/**********************************************************************/
#ifdef L_wcschrnul
#define L_strchrnul
#define Wstrchrnul wcschrnul
#else
#define Wstrchrnul strchrnul
#endif

#ifdef L_strchrnul

Wchar *Wstrchrnul(register const Wchar *s, Wint c)
{
	--s;
	while (*++s && (*s != ((Wchar)c)));
	return (Wchar *) s;
}

#endif
/**********************************************************************/
#ifdef L_rawmemchr

void *rawmemchr(const void *s, int c)
{
	register const unsigned char *r = s;

	while (*r != ((unsigned char)c)) ++r;

	return (void *) r;	/* silence the warning */
}

#endif
/**********************************************************************/
#ifdef L_basename

char *basename(const char *path)
{
	register const char *s;
	register const char *p;

	p = s = path;

	while (*s) {
		if (*s++ == '/') {
			p = s;
		}
	}

	return (char *) p;
}

#endif
/**********************************************************************/
#ifdef L___xpg_basename

char *__xpg_basename(register char *path)
{
	static const char null_or_empty[] = ".";
	char *first;
	register char *last;

	first = (char *) null_or_empty;

	if (path && *path) {
		first = path;
		last = path - 1;

		do {
			if ((*path != '/') && (path > ++last)) {
				last = first = path;
			}
		} while (*++path);

		if (*first == '/') {
			last = first;
		}
		last[1] = 0;
	}

	return first;
}

#endif
/**********************************************************************/
#ifdef L_dirname

char *dirname(char *path)
{
	static const char null_or_empty_or_noslash[] = ".";
	register char *s;
	register char *last;
	char *first;

	last = s = path;

	if (s != NULL) {

	LOOP:
		while (*s && (*s != '/')) ++s;
		first = s;
		while (*s == '/') ++s;
		if (*s) {
			last = first;
			goto LOOP;
		}

		if (last == path) {
			if (*last != '/') {
				goto DOT;
			}
			if ((*++last == '/') && (last[1] == 0)) {
				++last;
			}
		}
		*last = 0;
		return path;
	}
 DOT:
	return (char *) null_or_empty_or_noslash;
}

#endif
/**********************************************************************/
