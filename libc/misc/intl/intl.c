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
 *  Supply some weaks for gettext and friends.  Used by strerror*().
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#undef __OPTIMIZE__
#include <libintl.h>

/**********************************************************************/
#ifdef L_gettext

char *weak_function gettext(const char *msgid)
{
	return (char *) msgid;
}

#endif
/**********************************************************************/
#ifdef L_dgettext

char *__uClibc_dgettext(const char *domainname,
									  const char *msgid)
{
	return (char *) msgid;
}

weak_alias (__uClibc_dgettext, __dgettext)
weak_alias (__uClibc_dgettext, dgettext)

#endif
/**********************************************************************/
#ifdef L_dcgettext

char * __uClibc_dcgettext(const char *domainname,
									   const char *msgid, int category)
{
	return (char *) msgid;
}

weak_alias (__uClibc_dcgettext, __dcgettext)
weak_alias (__uClibc_dcgettext, dcgettext)

#endif
/**********************************************************************/
#ifdef L_ngettext

char *weak_function ngettext(const char *msgid1, const char *msgid2,
							 unsigned long int n)
{
	return (char *) ((n == 1) ? msgid1 : msgid2);
}

#endif
/**********************************************************************/
#ifdef L_dngettext

char *weak_function dngettext(const char *domainname, const char *msgid1,
							  const char *msgid2, unsigned long int n)
{
	return (char *) ((n == 1) ? msgid1 : msgid2);
}

#endif
/**********************************************************************/
#ifdef L_dcngettext

char *weak_function dcngettext(const char *domainname, const char *msgid1,
							   const char *msgid2, unsigned long int n,
							   int category)
{
	return (char *) ((n == 1) ? msgid1 : msgid2);
}

#endif
/**********************************************************************/
#ifdef L_textdomain

char *weak_function textdomain(const char *domainname)
{
	static const char default_str[] = "messages";

	if (domainname && *domainname && strcmp(domainname, default_str)) {
		__set_errno(EINVAL);
		return NULL;
	}
	return (char *) default_str;
}

#endif
/**********************************************************************/
#ifdef L_bindtextdomain

char *weak_function bindtextdomain(const char *domainname, const char *dirname)
{
	static const char dir[] = "/";

	if (!domainname || !*domainname
		|| (dirname
#if 1
			&& ((dirname[0] != '/') || dirname[1])
#else
			&& strcmp(dirname, dir)
#endif
			)
		) {
		__set_errno(EINVAL);
		return NULL;
	}

	return (char *) dir;
}

#endif
/**********************************************************************/
#ifdef L_bind_textdomain_codeset

/* Specify the character encoding in which the messages from the
   DOMAINNAME message catalog will be returned.  */
char *weak_function bind_textdomain_codeset(const char *domainname,
											const char *codeset)
{
	if (!domainname || !*domainname || codeset) {
		__set_errno(EINVAL);
	}
	return NULL;
}

#endif
/**********************************************************************/
