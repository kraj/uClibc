/* Copyright (C) 1995, 1996, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@gnu.ai.mit.edu <mailto:drepper@gnu.ai.mit.edu>>, August 1995.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>

/* Global state for non-reentrant functions.  */
struct drand48_data __libc_drand48_data;


#ifdef __UCLIBC_HAS_LONG_LONG__
int
__drand48_iterate (unsigned short int xsubi[3], struct drand48_data *buffer)
{
  uint64_t X;
  uint64_t result;

  /* Initialize buffer, if not yet done.  */
  if (unlikely(!buffer->__init))
    {
      buffer->__a = 0x5deece66dull;
      buffer->__c = 0xb;
      buffer->__init = 1;
    }

  /* Do the real work.  We choose a data type which contains at least
     48 bits.  Because we compute the modulus it does not care how
     many bits really are computed.  */

  X = (uint64_t) xsubi[2] << 32 | (uint32_t) xsubi[1] << 16 | xsubi[0];

  result = X * buffer->__a + buffer->__c;

  xsubi[0] = result & 0xffff;
  xsubi[1] = (result >> 16) & 0xffff;
  xsubi[2] = (result >> 32) & 0xffff;

  return 0;
}

#else
int
__drand48_iterate (unsigned short int xsubi[3], struct drand48_data *buffer)
{
  uint32_t X0, X1;
  uint32_t result0, result1;

  /* Initialize buffer, if not yet done.  */
  if (unlikely(!buffer->__init))
    {
      buffer->__a1 = 0x5;
      buffer->__a0 = 0xdeece66d;
      buffer->__c = 0xb;
      buffer->__init = 1;
    }

  /* Do the real work.  We choose a data type which contains at least
     48 bits.  Because we compute the modulus it does not care how
     many bits really are computed.  */

  /* X = X1*base32 + X0 */
  X1 = xsubi[2];
  X0 = xsubi[0] | (uint32_t) xsubi[1] << 16;

  result0 = buffer->__a0 * X0;
  result1 = (result0 > -buffer->__c ); /* Carry */
  /* If this overflows, the carry is already in result1 */
  result0 += buffer->__c;

  result1 += buffer->__a1*X0 + buffer->__a0*X1;

  xsubi[0] = result0 & 0xffff;
  xsubi[1] = result0 >> 16;
  xsubi[2] = result1 & 0xffff;

  return 0;
}
#endif
