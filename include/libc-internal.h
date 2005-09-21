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

/* Some nice features only work properly with ELF */
#if defined __HAVE_ELF__
/* Define ALIASNAME as a weak alias for NAME. */
#  define weak_alias(name, aliasname) _weak_alias (name, aliasname)
#  define _weak_alias(name, aliasname) \
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
#  if defined(__cris__) 
#    define link_warning(symbol, msg)
#  else
#    define link_warning(symbol, msg)					      \
	asm (".section "  ".gnu.warning." #symbol  "\n\t.previous");	      \
	    static const char __evoke_link_warning_##symbol[]		      \
	    __attribute__ ((unused, section (".gnu.warning." #symbol "\n\t#"))) = msg;
#endif
#else /* !defined __HAVE_ELF__ */
#  define strong_alias(name, aliasname) _strong_alias (name, aliasname)
#  define weak_alias(name, aliasname) _strong_alias (name, aliasname)
#  define _strong_alias(name, aliasname) \
	__asm__(".global " __C_SYMBOL_PREFIX__ #aliasname "\n" \
                ".set " __C_SYMBOL_PREFIX__ #aliasname "," __C_SYMBOL_PREFIX__ #name);
#  define link_warning(symbol, msg) \
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
#ifndef internal_function
# define internal_function      /* empty */
#endif

/* Prepare for the case that `__builtin_expect' is not available.  */
#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __builtin_expect(x, expected_value) (x)
#endif
#ifndef likely
# define likely(x)	__builtin_expect((!!(x)),1)
#endif
#ifndef unlikely
# define unlikely(x)	__builtin_expect((!!(x)),0)
#endif
#ifndef __LINUX_COMPILER_H
#define __LINUX_COMPILER_H
#endif
#ifndef __cast__
#define __cast__(_to)
#endif

/* Arrange to hide uClibc internals */
#if __GNUC_PREREQ (3, 3)
# define attribute_hidden __attribute__ ((visibility ("hidden")))
#else
# define attribute_hidden
#endif

/* --- this is added to integrate linuxthreads */
#define __USE_UNIX98            1

#endif /* _LIBC_INTERNAL_H */
