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

#ifndef __ASSEMBLER__
/* GCC understands weak symbols and aliases; use its interface where
   possible, instead of embedded assembly language.  */

/* Define ALIASNAME as a strong alias for NAME.  */
# define strong_alias(name, aliasname) _strong_alias(name, aliasname)
# define _strong_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((alias (#name)));

/* This comes between the return type and function name in
   a function definition to make that definition weak.  */
# define weak_function __attribute__ ((weak))
# define weak_const_function __attribute__ ((weak, __const__))

/* Define ALIASNAME as a weak alias for NAME.
   If weak aliases are not available, this defines a strong alias.  */
# define weak_alias(name, aliasname) _weak_alias (name, aliasname)
# define _weak_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));

/* Declare SYMBOL as weak undefined symbol (resolved to 0 if not defined).  */
# define weak_extern(symbol) _weak_extern (weak symbol)
# define _weak_extern(expr) _Pragma (#expr)

#else /* __ASSEMBLER__ */

#ifdef __SYMBOL_PREFIX
# define C_SYMBOL_NAME(name) _##name
#else
# define C_SYMBOL_NAME(name) name
#endif

# define strong_alias(name, aliasname)					\
  .global C_SYMBOL_NAME (aliasname) ;					\
  .set C_SYMBOL_NAME(aliasname),C_SYMBOL_NAME(name)

# define weak_alias(name, aliasname)					\
  .weak C_SYMBOL_NAME(aliasname) ;					\
  C_SYMBOL_NAME(aliasname) = C_SYMBOL_NAME(name)

# define weak_extern(symbol)						\
  .weak C_SYMBOL_NAME(symbol)

#endif /* __ASSEMBLER__ */

/* When a reference to SYMBOL is encountered, the linker will emit a
   warning message MSG.  */
#ifdef __HAVE_ELF__

/* We want the .gnu.warning.SYMBOL section to be unallocated.  */
# define __make_section_unallocated(section_string)	\
  asm (".section " section_string "\n\t.previous");

/* Tacking on "\n\t#" to the section name makes gcc put it's bogus
   section attributes on what looks like a comment to the assembler.  */
# define __sec_comment "\n\t#"
# ifdef __cris__
#  define link_warning(symbol, msg)
# else
#  define link_warning(symbol, msg) \
  __make_section_unallocated (".gnu.warning." #symbol) \
  static const char __evoke_link_warning_##symbol[]	\
    __attribute__ ((used, section (".gnu.warning." #symbol __sec_comment))) \
    = msg;
# endif
#else /* __HAVE_ELF__ */
# define link_warning(symbol, msg)		\
     asm (".stabs \"" msg "\",30,0,0,0\n\t"	\
          ".stabs \"" __C_SYMBOL_PREFIX__ #symbol "\",1,0,0,0\n");
#endif /* __HAVE_ELF__ */

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

#ifndef __ASSEMBLER__
# define hidden_strong_alias(name, aliasname) _hidden_strong_alias(name, aliasname)
# define _hidden_strong_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((alias (#name))) attribute_hidden;

# define hidden_weak_alias(name, aliasname) _hidden_weak_alias (name, aliasname)
# define _hidden_weak_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((weak, alias (#name))) attribute_hidden;
#else /* __ASSEMBLER__ */
# define hidden_strong_alias(name, aliasname)				\
  .global C_SYMBOL_NAME (aliasname) ;					\
  .hidden C_SYMBOL_NAME (aliasname) ;					\
  .set C_SYMBOL_NAME(aliasname),C_SYMBOL_NAME(name)

# define hidden_weak_alias(name, aliasname)				\
  .weak C_SYMBOL_NAME(aliasname) ;					\
  .hidden C_SYMBOL_NAME(aliasname) ;					\
  C_SYMBOL_NAME(aliasname) = C_SYMBOL_NAME(name)
#endif /* __ASSEMBLER__ */

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
#  ifdef __UCLIBC_HAS_WCHAR__
#   define __need_wchar_t
#   define __need_wint_t
#  endif
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
extern char *__strrchr (__const char *__s, int __c) attribute_hidden;
extern int __strncmp (__const char *__s1, __const char *__s2, size_t __n) attribute_hidden;
extern char *__strdup (__const char *__s) attribute_hidden;
extern int __strcasecmp (__const char *__s1, __const char *__s2) attribute_hidden;
extern int __strncasecmp (__const char *__s1, __const char *__s2, size_t __n) attribute_hidden;
extern void *__rawmemchr (__const void *__s, int __c) __THROW __attribute_pure__ __nonnull ((1)) attribute_hidden;
extern size_t __strspn (__const char *__s, __const char *__accept)
     __THROW __attribute_pure__ __nonnull ((1, 2)) attribute_hidden;
extern char *__strpbrk (__const char *__s, __const char *__accept)
     __THROW __attribute_pure__ __nonnull ((1, 2)) attribute_hidden;
extern size_t __strnlen (__const char *__string, size_t __maxlen)
     __THROW __attribute_pure__ __nonnull ((1)) attribute_hidden;
extern char *__strtok_r (char *__restrict __s, __const char *__restrict __delim,
		       char **__restrict __save_ptr) __THROW __nonnull ((2, 3)) attribute_hidden;

/* sources are built w/ _GNU_SOURCE, this gets undefined */
extern int __xpg_strerror_r_internal (int __errnum, char *__buf, size_t __buflen) attribute_hidden;
extern char *__glibc_strerror_r_internal (int __errnum, char *__buf, size_t __buflen) attribute_hidden;

/* ctype.h */
extern int __tolower (int __c) __THROW attribute_hidden;
extern int __toupper (int __c) __THROW attribute_hidden;

#ifdef __UCLIBC_HAS_WCHAR__
/* wchar.h */
extern size_t __wcslen (__const wchar_t *__s) __THROW __attribute_pure__ attribute_hidden;
extern wchar_t *__wcscpy (wchar_t *__restrict __dest, __const wchar_t *__restrict __src) __THROW attribute_hidden;
extern size_t __wcsspn (__const wchar_t *__wcs, __const wchar_t *__accept)
     __THROW __attribute_pure__ attribute_hidden;
extern wchar_t *__wcspbrk (__const wchar_t *__wcs, __const wchar_t *__accept)
     __THROW __attribute_pure__ attribute_hidden;
/* wctype.h */
extern wint_t __towlower (wint_t __wc) __THROW attribute_hidden;
#endif

/* #include <unistd.h> */
extern ssize_t __read(int __fd, void *__buf, size_t __nbytes) attribute_hidden;
extern ssize_t __write(int __fd, __const void *__buf, size_t __n) attribute_hidden;
extern int __close(int __fd) attribute_hidden;
extern __pid_t __getpid (void) attribute_hidden;
extern void _exit_internal (int __status) __attribute__ ((__noreturn__)) attribute_hidden;
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
#define abort __abort
#define fprintf __fprintf
#define fclose __fclose
#ifndef __USE_FILE_OFFSET64
#define fopen __fopen
#else
#define fopen __fopen64
#endif
#ifdef __USE_LARGEFILE64
#define fopen64 __fopen64
#endif

/* #include <stdlib.h> */
extern char *__getenv (__const char *__name) attribute_hidden;
extern void __exit (int __status) __THROW __attribute__ ((__noreturn__)) attribute_hidden;

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

/* internal access to program name */
extern const char *__uclibc_progname attribute_hidden;

# endif /* IS_IN_libc */
#endif /* __ASSEMBLER__ */

#endif /* _LIBC_INTERNAL_H */
