
#ifndef __FEATURES_H
#define __FEATURES_H


/* Major and minor version number of the uCLibc library package.  Use
   these macros to test for features in specific releases.  */
#define	__UCLIBC__		0
#define	__UCLIBC_MAJOR__	9
#define	__UCLIBC_MINOR__	1


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

#define _POSIX_THREAD_SAFE_FUNCTIONS

#include <sys/cdefs.h>


#endif

