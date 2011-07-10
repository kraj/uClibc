/* Copyright (C) 2002, 2003 Free Software Foundation, Inc.
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

#include <pthread.h>
#include <stdlib.h>	/* EXIT_SUCCESS */
#include <linuxthreads/sysdeps/pthread/pthread-functions.h>
/* for hidden __pthread_mutex_X */
#include <bits/uClibc_mutex.h>

/* Pointers to the libc functions.  */
struct pthread_functions __libc_pthread_functions;


# define FORWARD2(name, rettype, decl, params, defaction) \
rettype									      \
name decl								      \
{									      \
  if (__libc_pthread_functions.ptr_##name == NULL)			      \
    defaction;								      \
									      \
  return __libc_pthread_functions.ptr_##name params;			      \
}

# define FORWARD3(name, rettype, decl, params, defaction) \
rettype									      \
name decl								      \
{									      \
  if (__libc_pthread_functions.ptr_##name == NULL)			      \
    defaction;								      \
  while(1)								      \
    __libc_pthread_functions.ptr_##name params;				      \
}

# define FORWARD(name, decl, params, defretval) \
  FORWARD2 (name, int, decl, params, return defretval)

FORWARD (pthread_attr_destroy, (pthread_attr_t *attr), (attr), 0)

FORWARD (pthread_attr_init, (pthread_attr_t *attr), (attr), 0)

FORWARD (pthread_attr_getdetachstate,
	 (const pthread_attr_t *attr, int *detachstate), (attr, detachstate),
	 0)
FORWARD (pthread_attr_setdetachstate, (pthread_attr_t *attr, int detachstate),
	 (attr, detachstate), 0)

FORWARD (pthread_attr_getinheritsched,
	 (const pthread_attr_t *attr, int *inherit), (attr, inherit), 0)
FORWARD (pthread_attr_setinheritsched, (pthread_attr_t *attr, int inherit),
	 (attr, inherit), 0)

FORWARD (pthread_attr_getschedparam,
	 (const pthread_attr_t *attr, struct sched_param *param),
	 (attr, param), 0)
FORWARD (pthread_attr_setschedparam,
	 (pthread_attr_t *attr, const struct sched_param *param),
	 (attr, param), 0)

FORWARD (pthread_attr_getschedpolicy,
	 (const pthread_attr_t *attr, int *policy), (attr, policy), 0)
FORWARD (pthread_attr_setschedpolicy, (pthread_attr_t *attr, int policy),
	 (attr, policy), 0)

FORWARD (pthread_attr_getscope,
	 (const pthread_attr_t *attr, int *scope), (attr, scope), 0)
FORWARD (pthread_attr_setscope, (pthread_attr_t *attr, int scope),
	 (attr, scope), 0)


FORWARD (pthread_condattr_destroy, (pthread_condattr_t *attr), (attr), 0)
FORWARD (pthread_condattr_init, (pthread_condattr_t *attr), (attr), 0)


FORWARD (pthread_cond_broadcast, (pthread_cond_t *cond), (cond), 0)

FORWARD (pthread_cond_destroy, (pthread_cond_t *cond), (cond), 0)

FORWARD (pthread_cond_init,
	 (pthread_cond_t *cond, const pthread_condattr_t *cond_attr),
	 (cond, cond_attr), 0)

FORWARD (pthread_cond_signal, (pthread_cond_t *cond), (cond), 0)

FORWARD (pthread_cond_wait, (pthread_cond_t *cond, pthread_mutex_t *mutex),
	 (cond, mutex), 0)

FORWARD (pthread_cond_timedwait,
	 (pthread_cond_t *cond, pthread_mutex_t *mutex,
	  const struct timespec *abstime), (cond, mutex, abstime), 0)


FORWARD (pthread_equal, (pthread_t thread1, pthread_t thread2),
	 (thread1, thread2), 1)


/* Use an alias to avoid warning, as pthread_exit is declared noreturn.  */
/* Comment does not apply on uClibc, added FORWARD3 to handle this */
FORWARD3 (pthread_exit, void, (void *retval), (retval), exit (EXIT_SUCCESS))


FORWARD (pthread_getschedparam,
	 (pthread_t target_thread, int *policy, struct sched_param *param),
	 (target_thread, policy, param), 0)
FORWARD (pthread_setschedparam,
	 (pthread_t target_thread, int policy,
	  const struct sched_param *param), (target_thread, policy, param), 0)


FORWARD (pthread_mutex_destroy, (pthread_mutex_t *mutex), (mutex), 0)

FORWARD (pthread_mutex_init,
	 (pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr),
	 (mutex, mutexattr), 0)
strong_alias(pthread_mutex_init, __pthread_mutex_init)

FORWARD (pthread_mutex_lock, (pthread_mutex_t *mutex), (mutex), 0)
strong_alias(pthread_mutex_lock, __pthread_mutex_lock)

FORWARD (pthread_mutex_trylock, (pthread_mutex_t *mutex), (mutex), 0)
strong_alias(pthread_mutex_trylock, __pthread_mutex_trylock)

FORWARD (pthread_mutex_unlock, (pthread_mutex_t *mutex), (mutex), 0)
strong_alias(pthread_mutex_unlock, __pthread_mutex_unlock)

FORWARD2 (pthread_self, pthread_t, (void), (), return 0)


FORWARD (pthread_setcancelstate, (int state, int *oldstate), (state, oldstate),
	 0)

FORWARD (pthread_setcanceltype, (int type, int *oldtype), (type, oldtype), 0)

FORWARD2 (_pthread_cleanup_push, void, (struct _pthread_cleanup_buffer * buffer, void (*routine)(void *), void * arg), (buffer, routine, arg), return)
#undef _pthread_cleanup_push_defer
FORWARD2 (_pthread_cleanup_push_defer, void, (struct _pthread_cleanup_buffer * buffer, void (*routine)(void *), void * arg), (buffer, routine, arg), return)
strong_alias(_pthread_cleanup_push_defer,__pthread_cleanup_push_defer)

FORWARD2 (_pthread_cleanup_pop, void, (struct _pthread_cleanup_buffer * buffer, int execute), (buffer, execute), return)
#undef _pthread_cleanup_pop_restore
FORWARD2 (_pthread_cleanup_pop_restore, void, (struct _pthread_cleanup_buffer * buffer, int execute), (buffer, execute), return)
strong_alias(_pthread_cleanup_pop_restore,__pthread_cleanup_pop_restore)
