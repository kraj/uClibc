#ifndef __FEATURES_H
#define __FEATURES_H


/* Major and minor version number of the uClibc library package.  Use
   these macros to test for features in specific releases.  */
#define	__UCLIBC__		0
#define	__UCLIBC_MAJOR__	9
#define	__UCLIBC_MINOR__	5

/* Make a half-hearted attempt to accomodate non-gcc compilers */
#ifndef __GNUC__
#define __attribute(foo)  /* Ignore */
#endif

/* __restrict is known in EGCS 1.2 and above. */
#if !defined __GNUC__ || __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 92)
# define __restrict     /* Ignore */
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


#include <sys/cdefs.h>


#endif

