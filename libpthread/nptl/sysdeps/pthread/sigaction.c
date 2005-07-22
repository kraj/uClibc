/* Copyright (C) 2002, 2003, 2004 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2002.

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

/* This is tricky.  GCC doesn't like #include_next in the primary
   source file and even if it did, the first #include_next is this
   exact file anyway.  */
#ifndef LIBC_SIGACTION

#include <features.h>

#ifdef __UCLIBC__
#include <errno.h>
#include <signal.h>
#include <pthreadP.h>
#else
#include <nptl/pthreadP.h>
#endif

/* We use the libc implementation but we tell it to not allow
   SIGCANCEL or SIGTIMER to be handled.  */
# define LIBC_SIGACTION	1

#ifndef __UCLIBC__
# include <nptl/sysdeps/pthread/sigaction.c>

int
__sigaction (sig, act, oact)
     int sig;
     const struct sigaction *act;
     struct sigaction *oact;
#else
int
__sigaction (int sig, const struct sigaction *act, struct sigaction *oact)
#endif
{
  if (__builtin_expect (sig == SIGCANCEL || sig == SIGSETXID, 0))
    {
      __set_errno (EINVAL);
      return -1;
    }

#ifdef __UCLIBC__
  return sigaction (sig, act, oact);
#else
  return __libc_sigaction (sig, act, oact);
#endif
}
libc_hidden_weak (__sigaction)
#ifndef __UCLIBC__
weak_alias (__sigaction, sigaction)
#endif

#else

# include_next <sigaction.c>

#endif /* LIBC_SIGACTION */
