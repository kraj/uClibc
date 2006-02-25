/* vi: set sw=4 ts=4: */
/*
 * crypt() for uClibc
 *
 * Copyright (C) 2000 by Lineo, inc. and Erik Andersen
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
/* Written by Erik Andersen <andersen@uclibc.org> */

#define __FORCE_GLIBC
#include <crypt.h>
#include <unistd.h>

extern char * __md5_crypt( const char *pw, const char *salt) attribute_hidden;
extern char * __des_crypt( const char *pw, const char *salt) attribute_hidden;

char * crypt(const char *key, const char *salt)
{
	/* First, check if we are supposed to be using the MD5 replacement
	 * instead of DES...  */
	if (salt[0]=='$' && salt[1]=='1' && salt[2]=='$')
		return __md5_crypt(key, salt);
	else
		return __des_crypt(key, salt);
}
