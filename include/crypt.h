/*
 * UFC-crypt: ultra fast crypt(3) implementation
 *
 * Copyright (C) 1991, 92, 93, 96, 97, 98, 2000 Free Software Foundation, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the GNU C Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * @(#)crypt.h	1.5 12/20/96
 *
 */

#ifndef _CRYPT_H
#define _CRYPT_H	1

#include <features.h>

__BEGIN_DECLS

/* Encrypt characters from KEY using salt to perturb the encryption method.
 * If salt begins with "$1$", md5 hashing is used instead of DES. */
extern char *crypt (const char *__key, const char *__salt);

/* Setup DES tables according KEY.  */
extern void setkey (const char *__key);

/* Encrypt data in BLOCK in place if EDFLAG is zero; otherwise decrypt
   block in place.  */
extern void encrypt (char *__block, int __edflag);


/* Reentrant versions of the functions above.  The additional argument
   points to a structure where the results are placed in.  */
struct block {
	unsigned char b_data[64];
};
struct ordering {
	unsigned char o_data[64];
};
struct crypt_data
{
    /* Stuff used by the des based routines */
    struct block key;
    const struct ordering *EP;
    /* Stuff used by the md5 based routines */
    char *p;
    const char *sp,*ep;
};

extern char *crypt_r (const char *__key, const char *__salt,
		      struct crypt_data * __data);

extern void setkey_r (const char *__key,
		      struct crypt_data * __data);

extern void encrypt_r (char *__block, int __edflag,
		       struct crypt_data * __data);

__END_DECLS

#endif	/* crypt.h */
