/* ctype.c	
 * Character classification and conversion
 * Copyright (C) 2000 Lineo, Inc.
 * Written by Erik Andersen
 * This file is part of the uClibc C library and is distributed
 * under the GNU Library General Public License.
 */

#define USE_CTYPE_C_FUNCTIONS
#include <ctype.h>

#ifdef L_isalnum
int
isalnum( int c )
{
    return (isalpha(c) || isdigit(c));
}
#endif

#ifdef L_isalpha
int
isalpha( int c )
{
    return (isupper(c) || islower(c));
}
#endif

#ifdef L_isascii
int
isascii( int c )
{
    return (c > 0 && c <= 0x7f);
}
#endif

#ifdef L_iscntrl
int
iscntrl( int c )
{
    return ((c > 0) && ((c <= 0x1f) || (c == 0x7f)));
}
#endif

#ifdef L_isdigit
int
isdigit( int c )
{
    return (c >= '0' && c <= '9');
}
#endif

#ifdef L_isgraph
int
isgraph( int c )
{
    return (c != ' ' && isprint(c));
}
#endif

#ifdef L_islower
int
islower( int c )
{
    return (c >=  'a' && c <= 'z');
}
#endif

#ifdef L_isprint
int
isprint( int c )
{
    return (c >= ' ' && c <= '~');
}
#endif

#ifdef L_ispunct
int
ispunct( int c )
{
    return ((c > ' ' && c <= '~') && !isalnum(c));
}
#endif

#ifdef L_isspace
int
isspace( int c )
{
    return (c == ' ' || c == '\f' || c == '\n' || c == '\r' ||
	    c == '\t' || c == '\v');
}
#endif

#ifdef L_isupper
int
isupper( int c )
{
    return (c >=  'A' && c <= 'Z');
}
#endif

#ifdef L_isxdigit
int
isxdigit( int c )
{
    return (isxupper(c) || isxlower(c));
}
#endif

#ifdef L_isxlower
int
isxlower( int c )
{
    return (isdigit(c) || (c >= 'a' && c <= 'f'));
}
#endif

#ifdef L_isxupper
int
isxupper( int c )
{
    return (isdigit(c) || (c >= 'A' && c <= 'F'));
}
#endif

#ifdef L_toascii
int
toascii( int c )
{
    return (c & 0x7f);
}
#endif

#ifdef L_tolower
int
tolower( int c )
{
    return (isupper(c) ? ( c - 'A' + 'a') : (c));
}
#endif

#ifdef L_toupper
int
toupper( int c )
{
    return (islower(c) ? (c - 'a' + 'A') : (c));
}
#endif
