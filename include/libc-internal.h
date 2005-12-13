/* Copyright (C) 1991,92,93,95,96,97,98,99,2000,2001 Free Software Foundation, Inc.
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

#ifndef _LIBC_INTERNAL_H
#define _LIBC_INTERNAL_H 1

#include <features.h>

/* Some nice features only work properly with ELF */
#if defined __HAVE_ELF__
/* Define ALIASNAME as a weak alias for NAME. */
# define weak_alias(name, aliasname) _weak_alias (name, aliasname)
# define _weak_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));
/* Define ALIASNAME as a strong alias for NAME.  */
# define strong_alias(name, aliasname) _strong_alias(name, aliasname)
# define _strong_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((alias (#name)));
/* This comes between the return type and function name in
 *    a function definition to make that definition weak.  */
# define weak_function __attribute__ ((weak))
# define weak_const_function __attribute__ ((weak, __const__))
/* Tacking on "\n\t#" to the section name makes gcc put it's bogus
 * section attributes on what looks like a comment to the assembler. */
# if defined(__cris__) 
#   define link_warning(symbol, msg)
# else
#   define link_warning(symbol, msg)					      \
	asm (".section "  ".gnu.warning." #symbol  "\n\t.previous");	      \
	    static const char __evoke_link_warning_##symbol[]		      \
	    __attribute__ ((unused, section (".gnu.warning." #symbol "\n\t#"))) = msg;
# endif
#else /* !defined __HAVE_ELF__ */
# define strong_alias(name, aliasname) _strong_alias (name, aliasname)
# define weak_alias(name, aliasname) _strong_alias (name, aliasname)
# define _strong_alias(name, aliasname) \
	__asm__(".global " __C_SYMBOL_PREFIX__ #aliasname "\n" \
                ".set " __C_SYMBOL_PREFIX__ #aliasname "," __C_SYMBOL_PREFIX__ #name);
# define link_warning(symbol, msg) \
	asm (".stabs \"" msg "\",30,0,0,0\n\t" \
	      ".stabs \"" #symbol "\",1,0,0,0\n");
#endif

#ifndef weak_function
/* If we do not have the __attribute__ ((weak)) syntax, there is no way we
   can define functions as weak symbols.  The compiler will emit a `.globl'
   directive for the function symbol, and a `.weak' directive in addition
   will produce an error from the assembler.  */ 
# define weak_function          /* empty */
# define weak_const_function    /* empty */
#endif

/* On some platforms we can make internal function calls (i.e., calls of
   functions not exported) a bit faster by using a different calling
   convention.  */
#if 0 /*def __i386__*/
# define internal_function __attribute__ ((regparm (3), stdcall))
#endif
#ifndef internal_function
# define internal_function      /* empty */
#endif

#ifndef NOT_IN_libc
# define IS_IN_libc 1
#endif

/* Prepare for the case that `__builtin_expect' is not available.  */
#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
# define __builtin_expect(x, expected_value) (x)
#endif
#ifndef likely
# define likely(x)	__builtin_expect((!!(x)),1)
#endif
#ifndef unlikely
# define unlikely(x)	__builtin_expect((!!(x)),0)
#endif
#ifndef __LINUX_COMPILER_H
# define __LINUX_COMPILER_H
#endif
#ifndef __cast__
# define __cast__(_to)
#endif

#define attribute_unused __attribute__ ((unused))

/* Arrange to hide uClibc internals */
#if __GNUC_PREREQ (3, 3)
# define attribute_hidden __attribute__ ((visibility ("hidden")))
#else
# define attribute_hidden
#endif
#define hidden_def(name) extern __typeof (name) name attribute_hidden;
/* Define ALIASNAME as a hidden weak alias for NAME. */
# define hidden_weak_alias(name, aliasname) _hidden_weak_alias (name, aliasname)
# define _hidden_weak_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((weak, alias (#name))) __attribute__ ((visibility ("hidden")));
/* Define ALIASNAME as a hidden strong alias for NAME.  */
# define hidden_strong_alias(name, aliasname) _hidden_strong_alias(name, aliasname)
# define _hidden_strong_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((alias (#name))) __attribute__ ((visibility ("hidden")));

#ifdef __UCLIBC_BUILD_RELRO__
# define attribute_relro __attribute__ ((section (".data.rel.ro")))
#else
# define attribute_relro
#endif

#ifdef __GNUC__
# define attribute_noreturn __attribute__ ((__noreturn__))
#else
# define attribute_noreturn
#endif

/* Pull in things like __attribute_used__ */
#include <sys/cdefs.h>

/* --- this is added to integrate linuxthreads */
#define __USE_UNIX98            1

#ifndef __ASSEMBLER__
# ifdef IS_IN_libc

#  define __UC(N) __ ## N
#  define __UC_ALIAS(N) strong_alias( __ ## N , N )
#  if defined __UCLIBC_HAS_XLOCALE__ && defined __UCLIBC_DO_XLOCALE
#   define __UCXL(N) __ ## N ## _l
#   define __UCXL_ALIAS(N) strong_alias ( __ ## N ## _l , N ## _l )
#  else
#   define __UCXL(N) __UC(N)
#   define __UCXL_ALIAS(N) __UC_ALIAS(N)
#  endif

#  define __need_size_t
#  include <stddef.h>

#  include <bits/types.h>

#  ifndef __ssize_t_defined
typedef __ssize_t ssize_t;
#   define __ssize_t_defined
#  endif

#  include <bits/sigset.h>

/* prototypes for internal use, please keep these in sync w/ updated headers */
/* #include <fcntl.h> */
#ifndef __USE_FILE_OFFSET64
extern int __open (__const char *__file, int __oflag, ...) __nonnull ((1)) attribute_hidden;
extern int __fcntl (int __fd, int __cmd, ...) attribute_hidden;
#else
# ifdef __REDIRECT
extern int __REDIRECT (__open, (__const char *__file, int __oflag, ...), __open64)
     __nonnull ((1)) attribute_hidden;
extern int __REDIRECT (__fcntl, (int __fd, int __cmd, ...), __fcntl64) attribute_hidden;
# else
#  define __open __open64
#  define __fcntl __fcntl64
# endif
#endif
#ifdef __USE_LARGEFILE64
extern int __open64 (__const char *__file, int __oflag, ...) __nonnull ((1)) attribute_hidden;
extern int __fcntl64 (int __fd, int __cmd, ...) attribute_hidden;
#endif

/* #include <string.h> */
extern int __memcmp (__const void *__s1, __const void *__s2, size_t __n) attribute_hidden;
extern void *__memcpy (void *__restrict __dest,
		     __const void *__restrict __src, size_t __n) attribute_hidden;
extern void *__memmove (void *__dest, __const void *__src, size_t __n) attribute_hidden;
extern void *__memset (void *__s, int __c, size_t __n) attribute_hidden;
extern char *__strcpy (char *__restrict __dest, __const char *__restrict __src) attribute_hidden;
extern size_t __strlen (__const char *__s) attribute_hidden;
extern int __strcmp (__const char *__s1, __const char *__s2) attribute_hidden;
extern char *__strcat (char *__restrict __dest, __const char *__restrict __src) attribute_hidden;
extern char *__strncpy (char *__restrict __dest,
		      __const char *__restrict __src, size_t __n) attribute_hidden;
extern char *__strchr (__const char *__s, int __c) attribute_hidden;
extern int __strncmp (__const char *__s1, __const char *__s2, size_t __n) attribute_hidden;
extern char *__strdup (__const char *__s) attribute_hidden;
extern int __strcasecmp (__const char *__s1, __const char *__s2) attribute_hidden;
extern int __strncasecmp (__const char *__s1, __const char *__s2, size_t __n) attribute_hidden;

/* sources are built w/ _GNU_SOURCE, this gets undefined */
extern int __xpg_strerror_r_internal (int __errnum, char *__buf, size_t __buflen) attribute_hidden;
extern char *__glibc_strerror_r_internal (int __errnum, char *__buf, size_t __buflen) attribute_hidden;

/* #include <unistd.h> */
extern ssize_t __read(int __fd, void *__buf, size_t __nbytes) attribute_hidden;
extern ssize_t __write(int __fd, __const void *__buf, size_t __n) attribute_hidden;
extern int __close(int __fd) attribute_hidden;
extern __pid_t __getpid (void) attribute_hidden;
#ifndef __USE_FILE_OFFSET64
extern int __lockf (int __fd, int __cmd, __off_t __len) attribute_hidden;
extern __off_t __lseek (int __fd, __off_t __offset, int __whence) __THROW attribute_hidden;
#else
# ifdef __REDIRECT
extern int __REDIRECT (__lockf, (int __fd, int __cmd, __off64_t __len),
		       __lockf64) attribute_hidden;
extern __off64_t __REDIRECT (__lseek,
				 (int __fd, __off64_t __offset, int __whence),
				 __lseek64) attribute_hidden;
# else
#  define __lockf __lockf64
#  define __lseek __lseek64
# endif
#endif
#ifdef __USE_LARGEFILE64
extern int __lockf64 (int __fd, int __cmd, __off64_t __len) attribute_hidden;
extern __off64_t __lseek64 (int __fd, __off64_t __offset, int __whence) __THROW attribute_hidden;
#endif

/* #include <stdio.h> */
extern void __perror (__const char *__s) attribute_hidden;
extern int __printf (__const char *__restrict __format, ...) attribute_hidden;
extern int __sprintf (char *__restrict __s,
		    __const char *__restrict __format, ...) attribute_hidden;

/* hack */
#define fprintf __fprintf
#define fclose __fclose
#ifndef __USE_FILE_OFFSET64
#define fopen __fopen
#define readdir __readdir
#else
#define fopen __fopen64
#define readdir __readdir64
#endif
#ifdef __USE_LARGEFILE64
#define fopen64 __fopen64
#define readdir64 __readdir64
#endif

/* #include <stdlib.h> */
extern char *__getenv (__const char *__name) attribute_hidden;

/* #include <signal.h> */
extern int __sigprocmask (int __how, __const __sigset_t *__restrict __set,
			__sigset_t *__restrict __oset) attribute_hidden;

/* #include <sys/ioctl.h> */
extern int __ioctl (int __fd, unsigned long int __request, ...) attribute_hidden;

/* #include <sys/socket.h> */
extern int __socket (int __domain, int __type, int __protocol) attribute_hidden;

/* #include <sys/stat.h> */
#ifndef __USE_FILE_OFFSET64
struct stat;
extern int __stat (__const char *__restrict __file,
		 struct stat *__restrict __buf) __THROW __nonnull ((1, 2)) attribute_hidden;
extern int __fstat (int __fd, struct stat *__buf) __THROW __nonnull ((2)) attribute_hidden;
extern int __lstat (__const char *__restrict __file,
		  struct stat *__restrict __buf) __THROW __nonnull ((1, 2)) attribute_hidden;
#else
# ifdef __REDIRECT_NTH
extern int __REDIRECT_NTH (__stat, (__const char *__restrict __file,
				  struct stat *__restrict __buf), __stat64)
     __nonnull ((1, 2)) attribute_hidden;
extern int __REDIRECT_NTH (__fstat, (int __fd, struct stat *__buf), __fstat64)
     __nonnull ((2)) attribute_hidden;
extern int __REDIRECT_NTH (__lstat,
			   (__const char *__restrict __file,
			    struct stat *__restrict __buf), __lstat64)
     __nonnull ((1, 2)) attribute_hidden;
# else
#  define __stat __stat64
#  define __fstat __fstat64
#   define __lstat __lstat64
# endif
#endif
#ifdef __USE_LARGEFILE64
struct stat64;
extern int __stat64 (__const char *__restrict __file,
		   struct stat64 *__restrict __buf) __THROW __nonnull ((1, 2)) attribute_hidden;
extern int __fstat64 (int __fd, struct stat64 *__buf) __THROW __nonnull ((2)) attribute_hidden;
extern int __lstat64 (__const char *__restrict __file,
		    struct stat64 *__restrict __buf)
     __THROW __nonnull ((1, 2)) attribute_hidden;
#endif

/* #include <sys/statfs.h> */
#ifndef __USE_FILE_OFFSET64
struct statfs;
extern int __statfs (__const char *__file, struct statfs *__buf)
     __THROW __nonnull ((1, 2)) attribute_hidden;
extern int __fstatfs (int __fildes, struct statfs *__buf)
     __THROW __nonnull ((2)) attribute_hidden;
#else
# ifdef __REDIRECT
extern int __REDIRECT (__statfs,
			   (__const char *__file, struct statfs *__buf),
			   __statfs64) __nonnull ((1, 2)) attribute_hidden;
extern int __REDIRECT (__fstatfs, (int __fildes, struct statfs *__buf),
			   __fstatfs64) __nonnull ((2)) attribute_hidden;
# else
#  define __statfs __statfs64
# endif
#endif
#ifdef __USE_LARGEFILE64
struct statfs64;
extern int __statfs64 (__const char *__file, struct statfs64 *__buf)
     __THROW __nonnull ((1, 2)) attribute_hidden;
extern int __fstatfs64 (int __fildes, struct statfs64 *__buf)
     __THROW __nonnull ((2)) attribute_hidden;
#endif

#  if 0 /* undoable here */
/* #include <dirent.h> */
typedef struct __dirstream DIR;
extern DIR *__opendir (__const char *__name) attribute_hidden;
extern int __closedir (DIR *__dirp) attribute_hidden;

#ifndef __USE_FILE_OFFSET64
extern struct dirent *__readdir (DIR *__dirp) __nonnull ((1)) attribute_hidden;
#else
# ifdef __REDIRECT
extern struct dirent *__REDIRECT (__readdir, (DIR *__dirp), __readdir64)
     __nonnull ((1)) attribute_hidden;
# else
#  define __readdir __readdir64
# endif
#endif

#ifdef __USE_LARGEFILE64
extern struct dirent64 *__readdir64 (DIR *__dirp) __nonnull ((1)) attribute_hidden;
#endif

/* #include <stdio.h> */
extern int __vfprintf (FILE *__restrict __s, __const char *__restrict __format,
		     __gnuc_va_list __arg) attribute_hidden;
extern int __fprintf (FILE *__restrict __stream,
		    __const char *__restrict __format, ...) attribute_hidden;
extern int __fclose (FILE *__stream) attribute_hidden;

#ifndef __USE_FILE_OFFSET64
extern FILE *__fopen (__const char *__restrict __filename,
		    __const char *__restrict __modes) attribute_hidden;
#else
# ifdef __REDIRECT
extern FILE *__REDIRECT (__fopen, (__const char *__restrict __filename,
				 __const char *__restrict __modes), __fopen64) attribute_hidden;
# else
#  define __fopen __fopen64
# endif
#endif
#ifdef __USE_LARGEFILE64
extern FILE *__fopen64 (__const char *__restrict __filename,
		      __const char *__restrict __modes) attribute_hidden;
#endif

/* #include <sys/time.h> */
#   define __need_timeval
#   include <bits/time.h>
extern int __gettimeofday(struct timeval *__restrict __tv, *__restrict __timezone__ptr_t __tz) attribute_hidden;
#  endif

/* #include <pthread.h> */
#  ifndef __UCLIBC_HAS_THREADS__
#   define __pthread_mutex_init(mutex, mutexattr)         ((void)0)
#   define __pthread_mutex_lock(mutex)                    ((void)0)
#   define __pthread_mutex_trylock(mutex)                 ((void)0)
#   define __pthread_mutex_unlock(mutex)                  ((void)0)
#  endif

# endif /* IS_IN_libc */
#endif /* __ASSEMBLER__ */

#endif /* _LIBC_INTERNAL_H */
