/*
 * Copyright (C) 2005 Atmel Corporation
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License.  See the file "COPYING.LIB" in the main directory of this
 * archive for more details.
 */

#if !defined _BYTESWAP_H && !defined _NETINET_IN_H
# error "Never use <bits/byteswap.h> directly; include <byteswap.h> instead."
#endif

#ifndef _BITS_BYTESWAP_H
#define _BITS_BYTESWAP_H 1

/* Swap bytes in 16 bit value.  */
#if defined __GNUC__
# define __bswap_16(x) (__extension__ __builtin_bswap_16(x))
#else
/* This is better than nothing.  */
static __inline unsigned short int
__bswap_16 (unsigned short int __bsx)
{
	return ((((__bsx) >> 8) & 0xff) | (((__bsx) & 0xff) << 8));
}
#endif

/* Swap bytes in 32 bit value.  */
#if defined __GNUC__
# define __bswap_32(x) (__extension__ __builtin_bswap_32(x))
#else
static __inline unsigned int
__bswap_32 (unsigned int __bsx)
{
  return ((((__bsx) & 0xff000000) >> 24) | (((__bsx) & 0x00ff0000) >>  8) |
	  (((__bsx) & 0x0000ff00) <<  8) | (((__bsx) & 0x000000ff) << 24));
}
#endif

#if defined __GNUC__
/* Swap bytes in 64 bit value.  */
# define __bswap_constant_64(x)				\
	((((x) & 0xff00000000000000ull) >> 56)		\
	 | (((x) & 0x00ff000000000000ull) >> 40)	\
	 | (((x) & 0x0000ff0000000000ull) >> 24)	\
	 | (((x) & 0x000000ff00000000ull) >> 8)		\
	 | (((x) & 0x00000000ff000000ull) << 8)		\
	 | (((x) & 0x0000000000ff0000ull) << 24)	\
	 | (((x) & 0x000000000000ff00ull) << 40)	\
	 | (((x) & 0x00000000000000ffull) << 56))

# define __bswap_64(x)							\
	(__extension__							\
	 ({								\
		 union {						\
			 __extension__ unsigned long long int __ll;	\
			 unsigned int __l[2];				\
		 } __w, __r;						\
		 if (__builtin_constant_p(x))				\
			 __r.__ll = __bswap_constant_64(x);		\
		 else {							\
			 __w.__ll = (x);				\
			 __r.__l[0] = __bswap_32(__w.__l[1]);		\
			 __r.__l[1] = __bswap_32(__w.__l[0]);		\
		 }							\
		 __r.__ll;						\
	 }))
#endif

#endif /* _BITS_BYTESWAP_H */
