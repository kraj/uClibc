/*
 * libc/stdlib/malloc-zarg/calloc.c -- calloc function
 *
 *  Copyright (C) 2002  NEC Corporation
 *  Copyright (C) 2002  Miles Bader <miles@gnu.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License.  See the file COPYING.LIB in the main
 * directory of this archive for more details.
 * 
 * Written by Miles Bader <miles@gnu.org>
 */

#include <stdlib.h>
#include <string.h>

#include "malloc.h"


void *
calloc (size_t size, size_t num)
{
  void *mem;

  size *= num;

  mem = malloc (size);
  if (mem)
    memset (mem, 0, size);

  return mem;
}
