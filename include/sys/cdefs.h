
#ifndef __SYS_CDEFS_H
#define __SYS_CDEFS_H
#include <features.h>

#if defined (__STDC__) && __STDC__

#define	__CONCAT(x,y)	x ## y
#define	__STRING(x)	#x

/* This is not a typedef so `const __ptr_t' does the right thing.  */
#define __ptr_t void *

#else

#define	__CONCAT(x,y)	x/**/y
#define	__STRING(x)	"x"

#define __ptr_t char *

#endif

/* C++ needs to know that types and declarations are C, not C++.  */
#ifdef  __cplusplus
# define __BEGIN_DECLS  extern "C" {
# define __END_DECLS    }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

/* GNUish things */
#define __CONSTVALUE
#define __CONSTVALUE2

#ifdef __GNUC__
/* GCC can always grok prototypes.  For C++ programs we add throw()
   to help it optimize the function calls.  But this works only with
   gcc 2.8.x and egcs.  */
#if defined __cplusplus && __GNUC_PREREQ (2,8)
#define __THROW		throw()
#else
#define __THROW
#endif
#else /* GCC */
#define __THROW
#endif

#endif
