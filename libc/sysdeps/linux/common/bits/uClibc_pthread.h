/*  Copyright (C) 2003     Manuel Novoa III
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  The GNU C Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with the GNU C Library; if not, write to the Free
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307 USA.
 */

/* Supply prototypes for the (weak) thread functions used by the
 * uClibc library code.
 */

#ifndef _UCLIBC_PTHREAD_H
#define _UCLIBC_PTHREAD_H

#ifndef _PTHREAD_H
#error Always include <pthread.h> rather than <bits/uClibc_pthread.h>
#endif

extern int __pthread_mutex_init (pthread_mutex_t *__restrict __mutex,
								 __const pthread_mutexattr_t *__restrict
								 __mutex_attr) __THROW;

extern int __pthread_mutex_trylock (pthread_mutex_t *__mutex) __THROW;

extern int __pthread_mutex_lock (pthread_mutex_t *__mutex) __THROW;

extern int __pthread_mutex_unlock (pthread_mutex_t *__mutex) __THROW;

#endif
