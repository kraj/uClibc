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

/* Define the machine-dependent type `jmp_buf'.  m68k version.  */

#ifndef _SETJMP_H
# error "Never include <bits/setjmp.h> directly; use <setjmp.h> instead."
#endif

#include <signal.h>

typedef struct
  {
    /* There are eight 4-byte data registers, but D0 is not saved.  */
    long int __dregs[7];

    /* There are six 4-byte address registers, plus the FP and SP.  */
    int *__aregs[6];
    int *__fp;
    int *__sp;

#if defined __HAVE_68881__ || defined __HAVE_FPU__
    /* There are eight floating point registers which
       are saved in IEEE 96-bit extended format.  */
    char __fpregs[8 * (96 / 8)];
#endif

  } __jmp_buf[1];

/* Test if longjmp to JMPBUF would unwind the frame
   containing a local variable at ADDRESS.  */
#define _JMPBUF_UNWINDS(jmpbuf, address) \
  ((void *) (address) < (void *) (jmpbuf)->__sp)

/* Simple version of sigsetjmp and siglongjmp */

__BEGIN_DECLS
extern int __setjmp(__jmp_buf __buf);
extern int __longjmp(__jmp_buf __buf, int __val);
__END_DECLS

#define longjmp(buf, val) __longjmp(buf, val)

#define __sigsetjmp(env, savesigs) ((env)->__mask_was_saved = (savesigs), \
			sigprocmask(SIG_SETMASK, 0, &(env)->__saved_mask), \
			__setjmp((env)->__jmpbuf))

#define siglongjmp(env, val) (((env)->__mask_was_saved ? \
			sigprocmask(SIG_SETMASK, &(env)->__saved_mask, 0) : 0), \
			__longjmp((env)->__jmpbuf, val))

