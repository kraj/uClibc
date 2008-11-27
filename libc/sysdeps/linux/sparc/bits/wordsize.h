/* Determine the wordsize from the preprocessor defines.  */

#if defined __arch64__ || defined __sparcv9
# define __WORDSIZE	64
#else
# define __WORDSIZE	32
#endif

#if 0 /* uClibc: done in mathdefs.h: defined __UCLIBC_HAS_LONG_DOUBLE_MATH__ && !defined __LONG_DOUBLE_MATH_OPTIONAL*/

# if __WORDSIZE == 32
/* Signal that in 32bit ABI we didn't used to have a `long double'.
   The changes all the `long double' function variants to be redirects
   to the double functions.  */
#  define __LONG_DOUBLE_MATH_OPTIONAL   1
#  ifndef __LONG_DOUBLE_128__
#   undef __UCLIBC_HAS_LONG_DOUBLE_MATH__
#  endif
# endif
#endif
