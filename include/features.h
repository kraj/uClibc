#ifndef __FEATURES_H
#define __FEATURES_H


/* Major and minor version number of the uClibc library package.  Use
   these macros to test for features in specific releases.  */
#define	__UCLIBC__		0
#define	__UCLIBC_MAJOR__	9
#define	__UCLIBC_MINOR__	5

/*  There is an unwholesomely huge amount of code out there that depends on the
 *  presence of GNU libc header files.  We have GNU libc header files.  So here
 *  we commit a horrible sin.  At this point, we _lie_ and claim to be GNU libc
 *  to make things like /usr/include/linux/socket.h and lots of apps work as
 *  their developers intended.  This is IMHO, pardonable, since these defines
 *  are not really intended to check for the presence of a particular library,
 *  but rather are used to define an _interface_.  */
#if !defined _LIBC || defined __FORCE_GLIBC__ 
#   define __GNU_LIBRARY__ 6
#   define __GLIBC__       2
#   define __GLIBC_MINOR__ 1
#endif	

/* Make a half-hearted attempt to accomodate non-gcc compilers */
#ifndef __GNUC__
#define __attribute(foo)  /* Ignore */
#endif

/* Convenience macro to test the version of gcc.
 * Use it like this:
 * #if __GNUC_PREREQ (2,8)
 * ... code requiring gcc 2.8 or later ...
 * #endif
 * Note - they won't work for gcc1, since the _MINOR macros
 * were not defined then. */
#if defined __GNUC__ && defined __GNUC_MINOR__
#define __GNUC_PREREQ(maj, min) \
	((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#define __GNUC_PREREQ(maj,min) 0
#endif

/* __restrict is known in EGCS 1.2 and above. */
#if !defined __GNUC__ || __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 92)
# define __restrict     /* Ignore */
#endif

/* __extension__ is known in gcc 2.8 above. */
#if !defined __GNUC__ || __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 8)
# define __extension__     /* Ignore */
#endif

#ifdef __STDC__

#define __P(x)	    x
#define __PMT(x)    x
#ifndef __const
#define __const const
#endif

/* Almost ansi */
#if __STDC__ != 1
#ifndef const
#define const
#endif
#define volatile
#endif

#else /* K&R */

#define __P(x) ()
#ifndef __const
#define __const
#endif
#ifndef const
#define const
#endif
#define volatile

#endif

/* No C++ */
#define __BEGIN_DECLS
#define __END_DECLS

/* GNUish things */
#define __CONSTVALUE
#define __CONSTVALUE2

#define __USE_BSD
#define __USE_MISC
#define __USE_POSIX
#define __USE_POSIX2
#define __USE_XOPEN

#undef  __KERNEL_STRICT_NAMES
#ifndef _LOOSE_KERNEL_NAMES
# define __KERNEL_STRICT_NAMES
#endif

#ifdef  _GNU_SOURCE
# define __USE_GNU      1
#endif

#include <sys/cdefs.h>

#define __need_uClibc_config_h
#include <bits/uClibc_config.h>
#undef __need_uClibc_config_h


/* Some nice features only work properly with ELF */
#if defined HAVE_ELF	
#   define link_warning(symbol, msg) \
	asm (".section "  ".gnu.warning." #symbol  "\n\t.previous");  \
	    static const char __evoke_link_warning_##symbol[]     \
	    __attribute__ ((section (".gnu.warning." #symbol "\n\t#"))) = msg;
#   define weak_alias(name, aliasname) \
	extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));
#else
#   define link_warning(symbol, msg) \
	asm (".stabs \"" msg "\",30,0,0,0\n\t" \
	      ".stabs \"" #symbol "\",1,0,0,0\n");
#   define weak_alias(name, aliasname) \
	__asm__(".global alias\n.set alias,original");
#endif


#endif

