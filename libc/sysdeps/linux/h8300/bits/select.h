/* Copyright (C) 1997, 1998 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef _SYS_SELECT_H
# error "Never use <bits/select.h> directly; include <sys/select.h> instead."
#endif


#if defined __GNUC__ && __GNUC__ >= 2

# define __FD_ZERO(fdsp) \
  do {									      \
    __asm__ __volatile__ ("mov.l %1,er5\n\t"                                  \
                          "mov.l er5,er6\n\t"                                 \
                          "inc.l #1,er6\n\t"                                  \
			  "mov.b #0,r4l\n\t"                                  \
			  "mov.b r4l,@er5\n\t"                                \
			  "mov.l %0,er4\n\t"                                  \
			  "eepmov.w\n\t"                                      \
			  :: "r" (sizeof (fd_set)-1),  		              \
			     "r" (&__FDS_BITS (fdsp)[0])		      \
			  : "er4","er5","er6","memory");		      \
  } while (0)

# define __FD_SET(fd, fdsp) \
  __asm__ __volatile__ ("mov.l %0,er0\n\t"				      \
			"mov.l %1,er1\n\t"	      			      \
			"shlr.b r1l\n\t"	      			      \
			"shlr.b r1l\n\t"	      			      \
			"shlr.b r1l\n\t"	      			      \
			"add.l er1,er0\n\t"	      			      \
			"mov.l %1,er1\n\t"	      			      \
			"and.b #7,r1l\n\t"	      			      \
			"bset r1l,@er0\n\t"	      			      \
			: "=m" (__FDS_BITS (fdsp)[__FDELT (fd)])	      \
			: "r" (((int) (fd)) % __NFDBITS)		      \
			: "cc","memory","er0","er1")
# define __FD_CLR(fd, fdsp) \
  __asm__ __volatile__ ("mov.l %0,er0\n\t"				      \
			"mov.l %1,er1\n\t"	      			      \
			"shlr.b r1l\n\t"	      			      \
			"shlr.b r1l\n\t"	      			      \
			"shlr.b r1l\n\t"	      			      \
			"add.l er1,er0\n\t"	      			      \
			"mov.l %1,er1\n\t"	      			      \
			"and.b #7,r1l\n\t"	      			      \
			"bclr r1l,@er0\n\t"	      			      \
			: "=m" (__FDS_BITS (fdsp)[__FDELT (fd)])	      \
			: "r" (((int) (fd)) % __NFDBITS)		      \
			: "cc","memory","er0","er1")
# define __FD_ISSET(fd, fdsp) \
  (__extension__							      \
   ({register char __result;						      \
  __asm__ __volatile__ ("mov.l %1,er0\n\t"				      \
			"sub.l er1,er1\n\t"	      			      \
			"mov.w %0,r1\n\t"	      			      \
			"shlr.b r1l\n\t"	      			      \
			"shlr.b r1l\n\t"	      			      \
			"shlr.b r1l\n\t"	      			      \
			"add.l er1,er0\n\t"	      			      \
			"mov.w %0,r1\n\t"	      			      \
			"and.b #7,r1l\n\t"	      			      \
			"btst.b r1l,@er0\n\t"	      			      \
			"beq 1f\n\t"  	      			      \
			"sub.l er0,er0\n\t"  	      			      \
			"mov.b r1h,r0l\n\t"  	      			      \
			"bra 2f\n"  	      			      \
			"1:\tsub.l er0,er0\n" 	      			      \
			"2:\tmov.w r0,%0\n\t"  	      			      \
			: "=r" (__result)				      \
			: "r" (((int) (fd)) % __NFDBITS),		      \
			  "m" (__FDS_BITS (fdsp)[__FDELT (fd)])	      \
			: "cc");					      \
     __result; }))

#else	/* ! GNU CC */

/* We don't use `memset' because this would require a prototype and
   the array isn't too big.  */
# define __FD_ZERO(set)  \
  do {									      \
    unsigned int __i;							      \
    fd_set *__arr = (set);						      \
    for (__i = 0; __i < sizeof (fd_set) / sizeof (__fd_mask); ++__i)	      \
      __FDS_BITS (__arr)[__i] = 0;					      \
  } while (0)
# define __FD_SET(d, set)    (__FDS_BITS (set)[__FDELT (d)] |= __FDMASK (d))
# define __FD_CLR(d, set)    (__FDS_BITS (set)[__FDELT (d)] &= ~__FDMASK (d))
# define __FD_ISSET(d, set)  (__FDS_BITS (set)[__FDELT (d)] & __FDMASK (d))

#endif	/* GNU CC */



