
#ifndef __FEATURES_H
#define __FEATURES_H


/* Major and minor version number of the uCLibc library package.  Use
   these macros to test for features in specific releases.  */
#define	__UCLIBC__		0
#define	__UCLIBC_MAJOR__	9
#define	__UCLIBC_MINOR__	1

/* __restrict is known in EGCS 1.2 and above. */
#if !defined __GNUC__ || __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 92)
# define __restrict     /* Ignore */
#endif


#ifdef __STDC__

#define __P(x) x
#define __const const

/* Almost ansi */
#if __STDC__ != 1
#define const
#define volatile
#endif

#else /* K&R */

#define __P(x) ()
#define __const
#define const
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
#define _POSIX_THREAD_SAFE_FUNCTIONS

#include <sys/cdefs.h>


#endif

