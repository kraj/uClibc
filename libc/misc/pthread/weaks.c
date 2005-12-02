/* The weak pthread functions for Linux.
   Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.
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

#define _GNU_SOURCE
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

/**********************************************************************/
/* Weaks for application/library use.
 *
 * We need to define weaks here to cover all the pthread functions that
 * libc itself will use so that we aren't forced to link libc.so to
 * libpthread.so.  When an application doesn't use pthreads support,
 * the locking functions used by libc itself basically become no-ops.
 * However, if the application uses pthreads, then it will pull in
 * libpthread.so whose symbols will override these weaks magically
 * turning the internal libc mutex calls from no-ops to real locking
 * calls.
 */

/* glibc itself defines all these, but we don't need them in uClibc
 *
 * Verified by comparing to glibc's linuxthreads/forward.c and defined
 * only those that are in the glibc abi.
 * The commented aliases are ones that were previously defined in uClibc
 * and which I left in for documentation.
 */

static int __pthread_return_0 __P ((void));
static int __pthread_return_0 (void) { return 0; }

/*
weak_alias (__pthread_return_0, pthread_attr_destroy)
weak_alias (__pthread_return_0, pthread_attr_getdetachstate)
weak_alias (__pthread_return_0, pthread_attr_getinheritsched)
weak_alias (__pthread_return_0, pthread_attr_getschedparam)
weak_alias (__pthread_return_0, pthread_attr_getschedpolicy)
weak_alias (__pthread_return_0, pthread_attr_getscope)
weak_alias (__pthread_return_0, pthread_attr_getstackaddr)
weak_alias (__pthread_return_0, pthread_attr_getstacksize)
weak_alias (__pthread_return_0, pthread_attr_init)
weak_alias (__pthread_return_0, pthread_attr_setdetachstate)
weak_alias (__pthread_return_0, pthread_attr_setinheritsched)
weak_alias (__pthread_return_0, pthread_attr_setschedparam)
weak_alias (__pthread_return_0, pthread_attr_setschedpolicy)
weak_alias (__pthread_return_0, pthread_attr_setscope)
weak_alias (__pthread_return_0, pthread_attr_setstackaddr)
weak_alias (__pthread_return_0, pthread_attr_setstacksize)
weak_alias (__pthread_return_0, pthread_cond_broadcast)
weak_alias (__pthread_return_0, pthread_cond_destroy)
weak_alias (__pthread_return_0, pthread_cond_init)
weak_alias (__pthread_return_0, pthread_cond_signal)
weak_alias (__pthread_return_0, pthread_cond_timedwait)
weak_alias (__pthread_return_0, pthread_cond_wait)
weak_alias (__pthread_return_0, pthread_condattr_destroy)
weak_alias (__pthread_return_0, pthread_condattr_init)
weak_alias (__pthread_return_0, pthread_getschedparam)
weak_alias (__pthread_return_0, pthread_getcancelstate)
weak_alias (__pthread_return_0, pthread_getconcurrency)
weak_alias (__pthread_return_0, pthread_mutex_destroy)
weak_alias (__pthread_return_0, pthread_mutex_init)
weak_alias (__pthread_return_0, pthread_mutex_lock)
weak_alias (__pthread_return_0, pthread_mutex_trylock)
weak_alias (__pthread_return_0, pthread_mutex_unlock)
weak_alias (__pthread_return_0, pthread_mutexattr_destroy)
weak_alias (__pthread_return_0, pthread_mutexattr_gettype)
weak_alias (__pthread_return_0, pthread_mutexattr_init)
weak_alias (__pthread_return_0, pthread_mutexattr_settype)
weak_alias (__pthread_return_0, pthread_rwlock_destroy)
weak_alias (__pthread_return_0, pthread_rwlock_init)
weak_alias (__pthread_return_0, pthread_rwlock_rdlock)
weak_alias (__pthread_return_0, pthread_rwlock_tryrdlock)
weak_alias (__pthread_return_0, pthread_rwlock_trywrlock)
weak_alias (__pthread_return_0, pthread_rwlock_unlock)
weak_alias (__pthread_return_0, pthread_rwlock_wrlock)
weak_alias (__pthread_return_0, pthread_rwlockattr_destroy)
weak_alias (__pthread_return_0, pthread_rwlockattr_getpshared)
weak_alias (__pthread_return_0, pthread_rwlockattr_init)
weak_alias (__pthread_return_0, pthread_rwlockattr_setpshared)
weak_alias (__pthread_return_0, pthread_self)
weak_alias (__pthread_return_0, pthread_setcancelstate)
weak_alias (__pthread_return_0, pthread_setcanceltype)
weak_alias (__pthread_return_0, pthread_setconcurrency)
weak_alias (__pthread_return_0, pthread_setschedparam)

static int __pthread_return_1 __P ((void));
static int __pthread_return_1 (void) { return 1; }

weak_alias (__pthread_return_1, pthread_equal)

void weak_function pthread_exit (void *retval)
{
  exit (EXIT_SUCCESS);
}
*/

/**********************************************************************/
/* Weaks used internally by the C library. */
weak_alias (__pthread_return_0, __pthread_mutex_init)
weak_alias (__pthread_return_0, __pthread_mutex_lock)
weak_alias (__pthread_return_0, __pthread_mutex_trylock)
weak_alias (__pthread_return_0, __pthread_mutex_unlock)

/**********************************************************************/
