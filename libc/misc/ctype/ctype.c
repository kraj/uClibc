/* ctype.c
 * Character classification and conversion
 * Copyright (C) 2000 Lineo, Inc.
 * Written by Erik Andersen
 * This file is part of the uClibc C library and is distributed
 * under the GNU Library General Public License.
 *
 * not C-locale only code
 * written by Vladimir Oleynik (c) vodz@usa.net
 * and Manuel Novoa III <mnovoa3@bellsouth.net>
 * used ideas is part of the GNU C Library.
 */

#include <ctype.h>

#ifdef L_isascii
int
isascii( int c )
{
    return (c > 0 && c <= 0x7f);
}
#endif

#ifdef L_isdigit
int
isdigit( int c )
{
    return (c >= '0' && c <= '9');
}
#endif

#ifdef L_toascii
int
toascii( int c )
{
    return (c & 0x7f);
}
#endif


/* locale depended */
#ifndef __UCLIBC_HAS_LOCALE__

#ifdef L_isalpha
int
isalpha( int c )
{
    return (isupper(c) || islower(c));
}
#endif

#ifdef L_isalnum
int
isalnum( int c )
{
    return (isalpha(c) || isdigit(c));
}
#endif

#ifdef L_iscntrl
int
iscntrl( int c )
{
    return ((c >= 0) && ((c <= 0x1f) || (c == 0x7f)));
}
#endif

#ifdef L_isgraph
int
isgraph( int c )
{
    return (c > ' ' && isprint(c));
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
    return (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
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

#ifdef L_tolower
int
tolower( int c )
{
    return (isupper(c) ? (c - 'A' + 'a') : (c));
}
#endif

#ifdef L_toupper
int
toupper( int c )
{
    return (islower(c) ? (c - 'a' + 'A') : (c));
}
#endif

#else   /* __UCLIBC_HAS_LOCALE__ */

#include <limits.h>
#include "./ctype.h"

#define _UC_ISCTYPE(c, type) \
((c != -1) && ((_uc_ctype_b[(int)((unsigned char)c)] & type) != 0))

#define _UC_ISCTYPE2(c, type, type2) \
((c != -1) && ((_uc_ctype_b[(int)((unsigned char)c)] & type) == type2))


#ifdef L_ctype_C

/* startup setlocale(LC_TYPE, "C"); */
#include "ctype_C.c"

const unsigned char *_uc_ctype_b     = _uc_ctype_b_C;
const unsigned char *_uc_ctype_trans = _uc_ctype_b_C+LOCALE_BUF_SIZE/2;

#endif  /* L_ctype_C */

#ifdef L_isalpha
int
isalpha( int c )
{
    return _UC_ISCTYPE(c, ISalpha);
}
#endif

#ifdef L_isalnum
int
isalnum( int c )
{
    return _UC_ISCTYPE(c, (ISalpha|ISxdigit));
}
#endif

#ifdef L_iscntrl
int
iscntrl( int c )
{
    return _UC_ISCTYPE(c, IScntrl);
}
#endif

#ifdef L_isgraph
int
isgraph( int c )
{
    return _UC_ISCTYPE2(c, (ISprint|ISspace), ISprint);
}
#endif

#ifdef L_islower
int
islower( int c )
{
    return _UC_ISCTYPE(c, ISlower);
}
#endif

#ifdef L_isprint
int
isprint( int c )
{
    return _UC_ISCTYPE(c, ISprint);
}
#endif

#ifdef L_ispunct
int
ispunct( int c )
{
    return _UC_ISCTYPE(c, ISpunct);
}
#endif

#ifdef L_isspace
int
isspace( int c )
{
    return _UC_ISCTYPE(c, ISspace);
}
#endif

#ifdef L_isupper
int
isupper( int c )
{
    return _UC_ISCTYPE(c, ISupper);
}
#endif

#ifdef L_isxdigit
int
isxdigit( int c )
{
    return _UC_ISCTYPE(c, ISxdigit);
}
#endif

#ifdef L_isxlower
int
isxlower( int c )
{
    return _UC_ISCTYPE2(c, (ISxdigit|ISupper), ISxdigit);
}
#endif

#ifdef L_isxupper
int
isxupper( int c )
{
    return _UC_ISCTYPE2(c, (ISxdigit|ISlower), ISxdigit);
}
#endif

#ifdef L_tolower
int
tolower( int c )
{
    if((c < CHAR_MIN) || (c > UCHAR_MAX))
		return c;
    if(isupper(c))
		return _uc_ctype_trans[(int)((unsigned char)c)];
    else
		return c;
}
#endif

#ifdef L_toupper
int
toupper( int c )
{
    if((c < CHAR_MIN) || (c > UCHAR_MAX))
		return c;
    if(islower(c))
		return _uc_ctype_trans[(int)((unsigned char)c)];
    else
		return c;
}
#endif

#endif
