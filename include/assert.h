#ifndef	__ASSERT_H
#define	__ASSERT_H
#include <features.h>

/* If NDEBUG is defined, do nothing.
   If not, and EXPRESSION is zero, print an error message and abort.  */

#ifdef	NDEBUG

#define	assert(expr)		((void) 0)

#else /* Not NDEBUG.  */

extern void __assert __P((const char *, const char *, int, const char *));

#define	assert(expr)							      \
  ((void) ((expr) ||							      \
	   (__assert (__STRING(expr),				      \
			   __FILE__, __LINE__, __ASSERT_FUNCTION), 0)))

  
/* Version 2.4 and later of GCC define a magical variable `__PRETTY_FUNCTION__'
   which contains the name of the function currently being defined.
#  define __ASSERT_FUNCTION	__PRETTY_FUNCTION__
   This is broken in G++ before version 2.6.
   C9x has a similar variable called __func__, but prefer the GCC one since
   it demangles C++ function names.  */
# ifdef __GNUC__
#  if __GNUC__ > 2 || (__GNUC__ == 2 \
		       && __GNUC_MINOR__ >= (defined __cplusplus ? 6 : 4))
#   define __ASSERT_FUNCTION	__PRETTY_FUNCTION__
#  else
#   define __ASSERT_FUNCTION	((__const char *) 0)
#  endif
# else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define __ASSERT_FUNCTION	__func__
#  else
#   define __ASSERT_FUNCTION	((__const char *) 0)
#  endif
# endif


#endif /* NDEBUG.  */

#endif /* __ASSERT_H */
