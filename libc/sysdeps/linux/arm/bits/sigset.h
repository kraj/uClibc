/* __sig_atomic_t, __sigset_t, and related definitions.  Linux version.
   Copyright (C) 1991, 1992, 1994, 1996, 1997 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

#ifndef	_SIGSET_H_types
# define _SIGSET_H_types	1

typedef int __sig_atomic_t;

/* A `sigset_t' has a bit for each signal.  */

typedef unsigned long __sigset_t;

#endif


/* We only want to define these functions if <signal.h> was actually
   included; otherwise we were included just to define the types.  Since we
   are namespace-clean, it wouldn't hurt to define extra macros.  But
   trouble can be caused by functions being defined (e.g., any global
   register vars declared later will cause compilation errors).  */

#if !defined _SIGSET_H_fns && defined _SIGNAL_H
# define _SIGSET_H_fns 1

# ifndef _EXTERN_INLINE
#  define _EXTERN_INLINE extern __inline
# endif

/* Return a mask that includes the bit for SIG only.  */
# define __sigmask(sig) (1L << ((sig) - 1))

# if defined __GNUC__ && __GNUC__ >= 2
#  define __sigemptyset(set) (*(set) = 0)
#  define __sigfillset(set) (*(set) = ~0)

#  ifdef __USE_GNU
/* The POSIX does not specify for handling the whole signal set in one
   command.  This is often wanted and so we define three more functions
   here.  */
#   define __sigisemptyset(set) (*(set) == 0)
#   define __sigandset(dest, left, right) (*(dest) = *(left) & *(right))
#   define __sigorset(dest, left, right) (*(dest) = *(left) | *(right))
#  endif
# endif

/* These functions needn't check for a bogus signal number -- error
   checking is done in the non __ versions.  */

# define __sigismember(set, sig) (*(set) & (1L << ((sig)-1)))
# define __sigaddset(set, sig) (*(set) |= (1L << ((sig)-1)))
# define __sigdelset(set, sig) (*(set) &= ~(1L << ((sig)-1)))

#endif /* ! _SIGSET_H_fns.  */
