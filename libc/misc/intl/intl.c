/*  Copyright (C) 2003     Manuel Novoa III
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

/*
 *  Supply some weaks for use by strerror*(), etc.
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#undef __OPTIMIZE__
#include <libintl.h>

/**********************************************************************/
#ifdef L___uClibc_dgettext

char *__uClibc_dgettext(const char *domainname,
						const char *msgid)
{
	return (char *) msgid;
}

weak_alias (__uClibc_dgettext, __dgettext)

#endif
/**********************************************************************/
#ifdef L___uClibc_dcgettext

char * __uClibc_dcgettext(const char *domainname,
						  const char *msgid, int category)
{
	return (char *) msgid;
}

weak_alias (__uClibc_dcgettext, __dcgettext)

#endif
/**********************************************************************/
