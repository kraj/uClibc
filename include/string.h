
#ifndef __STRING_H
#define __STRING_H
#include <features.h>
#include <sys/types.h>
#include <stddef.h>

/* Basic string functions */

/* Return the length of S.  */
extern size_t strlen __P ((__const char *__s));
/* Append SRC onto DEST.  */
extern char *strcat __P ((char *__restrict __dest,
			  __const char *__restrict __src));
/* Append no more than N characters from SRC onto DEST.  */
extern char *strncat __P ((char *__restrict __dest,
			   __const char *__restrict __src, size_t __n));

/* Copy SRC to DEST.  */
extern char *strcpy __P ((char *__restrict __dest,
			  __const char *__restrict __src));
/* Copy no more than N characters of SRC to DEST.  */
extern char *strncpy __P ((char *__restrict __dest,
			   __const char *__restrict __src, size_t __n));

/* Compare S1 and S2.  */
extern int strcmp __P ((__const char *__s1, __const char *__s2));
/* Compare N characters of S1 and S2.  */
extern int strncmp __P ((__const char *__s1, __const char *__s2, size_t __n));

/* Find the first occurrence of C in S.  */
extern char *strchr __P ((__const char *__s, int __c));
/* Find the last occurrence of C in S.  */
extern char *strrchr __P ((__const char *__s, int __c));
/* Duplicate S, returning an identical malloc'd string.  */
extern char *strdup __P ((__const char *__s));

/* Basic mem functions */

/* Copy N bytes of SRC to DEST.  */
extern __ptr_t memcpy __P ((__ptr_t __restrict __dest,
			    __const __ptr_t __restrict __src, size_t __n));
/* Copy no more than N bytes of SRC to DEST, stopping when C is found.
   Return the position in DEST one byte past where C was copied,
   or NULL if C was not found in the first N bytes of SRC.  */
extern __ptr_t memccpy __P ((__ptr_t __dest, __const __ptr_t __src,
			     int __c, size_t __n));
/* Search N bytes of S for C.  */
extern __ptr_t memchr __P ((__const __ptr_t __s, int __c, size_t __n));
/* Set N bytes of S to C.  */
extern __ptr_t memset __P ((__ptr_t __s, int __c, size_t __n));
/* Compare N bytes of S1 and S2.  */
extern int memcmp __P ((__const __ptr_t __s1, __const __ptr_t __s2,
			size_t __n));
/* Copy N bytes of SRC to DEST, guaranteeing
   correct behavior for overlapping strings.  */
extern __ptr_t memmove __P ((__ptr_t __dest, __const __ptr_t __src,
			     size_t __n));


/* Minimal (very!) locale support */
#define strcoll strcmp
#define strxfrm strncpy

/* BSDisms */
#define index strchr
#define rindex strrchr

/* Other common BSD functions */
/* Set N bytes of S to 0.  */
extern void bzero __P ((__ptr_t __s, size_t __n));
/* Copy N bytes of SRC to DEST (like memmove, but args reversed).  */
extern void bcopy __P ((__const __ptr_t __src, __ptr_t __dest, size_t __n));

/* Compare S1 and S2, ignoring case.  */
extern int strcasecmp __P ((__const char *__s1, __const char *__s2));
/* Compare no more than N chars of S1 and S2, ignoring case.  */
extern int strncasecmp __P ((__const char *__s1, __const char *__s2,
			     size_t __n));
/* Find the first occurrence in S of any character in ACCEPT.  */
extern char *strpbrk __P ((__const char *__s, __const char *__accept));
/* Return the next DELIM-delimited token from *STRINGP,
   terminating it with a '\0', and update *STRINGP to point past it.  */
extern char *strsep __P ((char **__restrict __stringp,
			  __const char *__restrict __delim));
/* Find the first occurrence of NEEDLE in HAYSTACK.  */
extern char *strstr __P ((__const char *__haystack, __const char *__needle));
/* Divide S into tokens separated by characters in DELIM.  */
extern char *strtok __P ((char *__restrict __s,
			  __const char *__restrict __delim));
/* Return the length of the initial segment of S which
   consists entirely of characters not in REJECT.  */
extern size_t strcspn __P ((__const char *__s, __const char *__reject));
/* Return the length of the initial segment of S which
   consists entirely of characters in ACCEPT.  */
extern size_t strspn __P ((__const char *__s, __const char *__accept));


/* More BSD compatabilty */
#define bcmp	memcmp

/* Linux silly hour */
char *strfry __P ((char *));

#endif
