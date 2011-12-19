/* vi: set sw=4 ts=4: */
/*
 * crypt() for uClibc
 * Copyright (C) 2000-2006 by Erik Andersen <andersen@uclibc.org>
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define __FORCE_GLIBC
#include <crypt.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "libcrypt.h"

typedef char *(*crypt_impl_f)(const unsigned char *pw, const unsigned char *salt);

static const struct {
	const char *salt_pfx;
	const crypt_impl_f crypt_impl;
} crypt_impl_tab[] = {
	{ "$1$",        __md5_crypt },
#ifdef __UCLIBC_HAS_SHA512_CRYPT_IMPL__
	{ "$6$",        __sha512_crypt },
#endif
	{ NULL,         __des_crypt },
};

char *crypt(const char *key, const char *salt)
{
	const unsigned char *ukey = (const unsigned char *)key;
	const unsigned char *usalt = (const unsigned char *)salt;
	size_t i;

	for (i = 0; i < ARRAY_SIZE(crypt_impl_tab); i++) {
		if (crypt_impl_tab[i].salt_pfx != NULL &&
		    strncmp(crypt_impl_tab[i].salt_pfx, salt, strlen(crypt_impl_tab[i].salt_pfx)))
			continue;

		return crypt_impl_tab[i].crypt_impl(ukey, usalt);
	}

	/* no crypt implementation was found, set errno to ENOSYS and return NULL */
	__set_errno(ENOSYS);
	return NULL;
}
