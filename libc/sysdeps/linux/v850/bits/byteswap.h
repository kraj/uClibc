/*
 * libc/sysdeps/linux/v850/bits/byteswap.h -- Macros to swap the order
 * 	of bytes in integer values
 *
 *  Copyright (C) 2001  NEC Corporation
 *  Copyright (C) 2001  Miles Bader <miles@gnu.org>
 *  Copyright (C) 1997,1998,2001  Free Software Foundation, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License.  See the file COPYING.LIB in the main
 * directory of this archive for more details.
 */

#if !defined _BYTESWAP_H && !defined _NETINET_IN_H
# error "Never use <bits/byteswap.h> directly; include <byteswap.h> instead."
#endif

/* Swap bytes in 16 bit value.  */
#ifdef __GNUC__
# define __bswap_16(x)							      \
    (__extension__							      \
     ({ unsigned short int __bsh_val = (x);				      \
        __asm__ ("bsh %1, %0" : "=r" (__bsh_val)  : "r" (__bsh_val));	      \
	__bsh_val; }))
#else
static __inline unsigned short int
__bswap_16 (unsigned short int __bsx)
{
  return ((((__bsx) >> 8) & 0xff) | (((__bsx) & 0xff) << 8));
}
#endif

/* Swap bytes in 32 bit value.  */
#ifdef __GNUC__
# define __bswap_32(x)							      \
    (__extension__							      \
     ({ unsigned short int __bsw_val = (x);				      \
        __asm__ ("bsw %1, %0" : "=r" (__bsw_val)  : "r" (__bsw_val));	      \
	__bsw_val; }))
#else
static __inline unsigned int
__bswap_32 (unsigned int __bsx)
{
  return ((((__bsx) & 0xff000000) >> 24) | (((__bsx) & 0x00ff0000) >>  8) |
	  (((__bsx) & 0x0000ff00) <<  8) | (((__bsx) & 0x000000ff) << 24));
}
#endif

#if defined __GNUC__ && __GNUC__ >= 2
/* Swap bytes in 64 bit value.  */
# define __bswap_64(x) \
     (__extension__							      \
      ({ union { unsigned long long int __ll;				      \
		 unsigned long int __l[2]; } __v, __r;			      \
	 __v.__ll = (x);						      \
	 __r.__l[0] = __bswap_32 (__v.__l[1]);				      \
	 __r.__l[1] = __bswap_32 (__v.__l[0]);				      \
	 __r.__ll; }))
#endif
