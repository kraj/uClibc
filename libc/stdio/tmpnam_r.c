/* vi: set sw=4 ts=4: */
/*
 * tmpnam for uClibc
 *
 * Copyright (C) 2000 by Lineo, inc.  Written by Erik Andersen
 * <andersen@lineo.com>, <andersee@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Modified by Erik Andersen <anderse@debian.org> to be reentrant for
 * the case when S != NULL...
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


/* Generate a unique filename in /tmp.
 * If s is NULL return NULL, making this function thread safe.  */

char * tmpnam_r (char *s)
{
	if (s == NULL)
		return NULL;
	else
		return (tmpnam(s));
}

