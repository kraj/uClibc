/* ctype.h
 * Character classification and conversion */

#ifndef __CTYPE_H
#define __CTYPE_H

#include <features.h>

/* For now, just always use the functions instead of the macros...*/
#define __USE_CTYPE_C_FUNCTIONS

/* Locale-compatible macros/inlines have yet to be implemented. */
#if __UCLIBC_HAS_LOCALE__ && !defined(__USE_CTYPE_C_FUNCTIONS)
#define __USE_CTYPE_C_FUNCTIONS
#endif

#ifdef __USE_CTYPE_C_FUNCTIONS
/* function prototpes */ 
extern int isalnum(int c);
extern int isalpha(int c);
extern int isascii(int c);
extern int iscntrl(int c);
extern int isdigit(int c);
extern int isgraph(int c);
extern int islower(int c);
extern int isprint(int c);
extern int ispunct(int c);
extern int isspace(int c);
extern int isupper(int c);
extern int isxdigit(int c);
extern int isxlower(int c);
extern int isxupper(int c);
extern int toascii(int c);
extern int tolower(int c);
extern int toupper(int c);

#else

/* macro definitions */
#define isalnum(c)  (isalpha(c) || isdigit(c))
#define isalpha(c)  (isupper(c) || islower(c))
#define isascii(c)  (c > 0 && c <= 0x7f)
#define iscntrl(c)  ((c >= 0) && ((c <= 0x1F) || (c == 0x7f)))
#define isdigit(c)  (c >= '0' && c <= '9')
#define isgraph(c)  (c != ' ' && isprint(c))
#define islower(c)  (c >=  'a' && c <= 'z')
#define isprint(c)  (c >= ' ' && c <= '~')
#define ispunct(c)  ((c > ' ' && c <= '~') && !isalnum(c))
#define isspace(c)  (c == ' ' || c == '\f' || c == '\n' || c == '\r' ||\
			c == '\t' || c == '\v')
#define isupper(c)  (c >=  'A' && c <= 'Z')
#define isxdigit(c) (isxupper(c) || isxlower(c))
#define isxlower(c) (isdigit(c) || (c >= 'a' && c <= 'f'))
#define isxupper(c) (isdigit(c) || (c >= 'A' && c <= 'F'))
#define toascii(c)  (c & 0x7f)
#define tolower(c)  (isupper(c) ? ( c - 'A' + 'a') : (c))
#define toupper(c)  (islower(c) ? (c - 'a' + 'A') : (c))

#endif

#endif /* __CTYPE_H */
