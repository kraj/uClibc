/* Machine-dependent pthreads configuration and inline functions.
   ARM version.
   Copyright (C) 1997, 1998, 2000, 2002, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Alexandre Oliva <aoliva@redhat.com>

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef _PT_MACHINE_H
#define _PT_MACHINE_H   1

#ifndef PT_EI
# define PT_EI extern inline
#endif

/* Get some notion of the current stack.  Need not be exactly the top
   of the stack, just something somewhere in the current frame.  */
#define CURRENT_STACK_FRAME  __builtin_frame_address (0)


extern long int testandset (int *spinlock);

/* Spinlock implementation; required.  */
PT_EI long int
testandset (int *spinlock)
{
  register long int ret = 1;

  __asm__ __volatile__("swap%I1\t%M1,%0"
		       : "+r"(ret), "+m"(*spinlock));

  return ret;
}

#endif /* pt-machine.h */
