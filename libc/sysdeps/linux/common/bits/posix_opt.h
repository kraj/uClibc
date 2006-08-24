/* Define POSIX options for Linux.
   Copyright (C) 1996,1997,1999,2000,2002,2003 Free Software Foundation, Inc.
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

/*
 * Never include this file directly; use <unistd.h> instead.
 */

#ifndef	_BITS_POSIX_OPT_H
#define	_BITS_POSIX_OPT_H	1

/* Job control is supported.  */
#define	_POSIX_JOB_CONTROL	1

/* Processes have a saved set-user-ID and a saved set-group-ID.  */
#define	_POSIX_SAVED_IDS	1

/* Priority scheduling is supported.  */
#define	_POSIX_PRIORITY_SCHEDULING	200112L

/* Synchronizing file data is supported.  */
#define	_POSIX_SYNCHRONIZED_IO	200112L

/* The fsync function is present.  */
#define	_POSIX_FSYNC	200112L

/* Mapping of files to memory is supported.  */
#define	_POSIX_MAPPED_FILES	200112L

/* Locking of all memory is supported.  */
#ifdef __ARCH_USE_MMU__
# define	_POSIX_MEMLOCK	200112L
#else
# undef		_POSIX_MEMLOCK
#endif

/* Locking of ranges of memory is supported.  */
#ifdef __ARCH_USE_MMU__
# define	_POSIX_MEMLOCK_RANGE	200112L
#else
# define	_POSIX_MEMLOCK_RANGE
#endif

/* Setting of memory protections is supported.  */
#ifdef __ARCH_USE_MMU__
# define	_POSIX_MEMORY_PROTECTION	200112L
#else
# undef		_POSIX_MEMORY_PROTECTION
#endif

/* Implementation supports `poll' function.  */
#define	_POSIX_POLL	1

/* Implementation supports `select' and `pselect' functions.  */
#define	_POSIX_SELECT	1

/* Only root can change owner of file.  */
#define	_POSIX_CHOWN_RESTRICTED	1

/* `c_cc' member of 'struct termios' structure can be disabled by
   using the value _POSIX_VDISABLE.  */
#define	_POSIX_VDISABLE	'\0'

/* Filenames are not silently truncated.  */
#define	_POSIX_NO_TRUNC	1

/* X/Open realtime support is available.  */
#define _XOPEN_REALTIME	1

/* X/Open realtime thread support is available.  */
#ifdef __UCLIBC_HAS_THREADS__
# define _XOPEN_REALTIME_THREADS	1
#else
# undef _XOPEN_REALTIME_THREADS
#endif

/* XPG4.2 shared memory is supported.  */
#define	_XOPEN_SHM	1

/* Tell we have POSIX threads.  */
#ifdef __UCLIBC_HAS_THREADS__
# define _POSIX_THREADS	1
#else
# undef _POSIX_THREADS
#endif

/* We have the reentrant functions described in POSIX.  */
#ifdef __UCLIBC_HAS_THREADS__
# define _POSIX_REENTRANT_FUNCTIONS	1
# define _POSIX_THREAD_SAFE_FUNCTIONS	1
#else
# undef _POSIX_REENTRANT_FUNCTIONS
# undef _POSIX_THREAD_SAFE_FUNCTIONS
#endif

/* We provide priority scheduling for threads.  */
#define _POSIX_THREAD_PRIORITY_SCHEDULING	1

/* We support user-defined stack sizes.  */
#define _POSIX_THREAD_ATTR_STACKSIZE	1

/* We support user-defined stacks.  */
#define _POSIX_THREAD_ATTR_STACKADDR	1

/* We support POSIX.1b semaphores, but only the non-shared form for now.  */
#ifdef __UCLIBC_HAS_THREADS__
# define _POSIX_SEMAPHORES	1
#else
# undef _POSIX_SEMAPHORES
#endif

/* Real-time signals are supported.  */
#define _POSIX_REALTIME_SIGNALS	200112L

/* We support asynchronous I/O.  */
#define _POSIX_ASYNCHRONOUS_IO	1
#define _POSIX_ASYNC_IO		1
/* Alternative name for Unix98.  */
#define _LFS_ASYNCHRONOUS_IO	1

/* The LFS support in asynchronous I/O is also available.  */
#ifdef __UCLIBC_HAS_LFS__
# define _LFS64_ASYNCHRONOUS_IO	1
#else
# undef _LFS64_ASYNCHRONOUS_IO
#endif

/* The rest of the LFS is also available.  */
#ifdef __UCLIBC_HAS_LFS__
# define _LFS_LARGEFILE		1
# define _LFS64_LARGEFILE	1
# define _LFS64_STDIO		1
#else
# undef _LFS_LARGEFILE
# undef _LFS64_LARGEFILE
# undef _LFS64_STDIO
#endif

/* POSIX timers are available.  */
#define _POSIX_TIMERS	200112L

/* POSIX shared memory objects are implemented.  */
#define _POSIX_SHARED_MEMORY_OBJECTS	200112L

#if defined(__i386__)
/* CPU-time clocks supported.  */
# define _POSIX_CPUTIME 200912L

/* We support the clock also in threads.  */
# define _POSIX_THREAD_CPUTIME  200912L
#endif

#ifdef __UCLIBC_HAS_THREADS_NATIVE__
/* CPU-time clocks support needs to be checked at runtime.  */
#define _POSIX_CPUTIME  0

/* Clock support in threads must be also checked at runtime.  */
#define _POSIX_THREAD_CPUTIME   0
#endif

/* Reader/Writer locks are available.  */
#define _POSIX_READER_WRITER_LOCKS	200912L

/* GNU libc provides regular expression handling.  */
#ifdef __UCLIBC_HAS_REGEX__
# define _POSIX_REGEXP	1
#else
# undef _POSIX_REGEXP
#endif

/* We have a POSIX shell.  */
#define _POSIX_SHELL	1

/* The `spawn' function family is supported.  */
#define _POSIX_SPAWN	200912L

/* We support the Timeouts option.  */
#define _POSIX_TIMEOUTS	200912L

/* We support spinlocks.  */
#define _POSIX_SPIN_LOCKS	200912L

/* The barrier functions are available.  */
#define _POSIX_BARRIERS	200912L

#ifdef __UCLIBC_HAS_THREADS_NATIVE__
/* POSIX message queues are available.  */
# define _POSIX_MESSAGE_PASSING  200112L
#else
/* POSIX message queues are not yet supported.  */
# undef	_POSIX_MESSAGE_PASSING
#endif

#ifdef __UCLIBC_HAS_THREADS_NATIVE__
/* Thread process-shared synchronization is supported.  */
#define _POSIX_THREAD_PROCESS_SHARED    200112L

/* The monotonic clock might be available.  */
#define _POSIX_MONOTONIC_CLOCK  0

/* The clock selection interfaces are available.  */
#define _POSIX_CLOCK_SELECTION  200112L
#endif

#endif /* bits/posix_opt.h */
