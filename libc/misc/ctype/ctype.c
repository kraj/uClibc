/* ctype.c	
 * Character classification and conversion
 * Copyright (C) 2000 Lineo, Inc.
 * Written by Erik Andersen
 * This file is part of the uC-Libc C library and is distributed
 * under the GNU Library General Public License.
 */

#define USE_CTYPE_C_FUNCTIONS
#include <ctype.h>

int
isalnum( int c )
{
    return (isalpha(c) || isdigit(c));
}

int
isalpha( int c )
{
    return (isupper(c) || islower(c));
}

int
isascii( int c )
{
    return (c > 0 && c <= 0x7f);
}

int
iscntrl( int c )
{
    return ((c > 0) && ((c <= 0x1f) || (c == 0x7f)));
}

int
isdigit( int c )
{
    return (c >= '0' && c <= '9');
}

int
isgraph( int c )
{
    return (c != ' ' && isprint(c));
}

int
islower( int c )
{
    return (c >=  'a' && c <= 'z');
}

int
isprint( int c )
{
    return (c >= ' ' && c <= '~');
}

int
ispunct( int c )
{
    return ((c > ' ' && c <= '~') && !isalnum(c));
}

int
isspace( int c )
{
    return (c == ' ' || c == '\f' || c == '\n' || c == '\r' ||
	    c == '\t' || c == '\v');
}

int
isupper( int c )
{
    return (c >=  'A' && c <= 'Z');
}

int
isxdigit( int c )
{
    return (isxupper(c) || isxlower(c));
}

int
isxlower( int c )
{
    return (isdigit(c) || (c >= 'a' && c <= 'f'));
}

int
isxupper( int c )
{
    return (isdigit(c) || (c >= 'A' && c <= 'F'));
}

int
toascii( int c )
{
    return (c & 0x7f);
}

int
tolower( int c )
{
    return (isupper(c) ? ( c - 'A' + 'a') : (c));
}

int
toupper( int c )
{
    return (islower(c) ? (c - 'a' + 'A') : (c));
}

