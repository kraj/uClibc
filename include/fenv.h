/*
 *
 * Copyright (c) 2007  STMicroelectronics Ltd
 * Filippo Arcidiacono (filippo.arcidiacono@st.com)
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 * Taken from glibc 2.6
 *
 */


/*
 * ISO C99 7.6: Floating-point environment	<fenv.h>
 */

#ifndef _FENV_H
#define _FENV_H 1

#include <features.h>

/* Get the architecture dependend definitions.  The following definitions
   are expected to be done:

   fenv_t	type for object representing an entire floating-point
		environment

   FE_DFL_ENV	macro of type pointer to fenv_t to be used as the argument
		to functions taking an argument of type fenv_t; in this
		case the default environment will be used

   fexcept_t	type for object representing the floating-point exception
		flags including status associated with the flags

   The following macros are defined iff the implementation supports this
   kind of exception.
   FE_INEXACT		inexact result
   FE_DIVBYZERO		division by zero
   FE_UNDERFLOW		result not representable due to underflow
   FE_OVERFLOW		result not representable due to overflow
   FE_INVALID		invalid operation

   FE_ALL_EXCEPT	bitwise OR of all supported exceptions

   The next macros are defined iff the appropriate rounding mode is
   supported by the implementation.
   FE_TONEAREST		round to nearest
   FE_UPWARD		round toward +Inf
   FE_DOWNWARD		round toward -Inf
   FE_TOWARDZERO	round toward 0
*/
#include <bits/fenv.h>

__BEGIN_DECLS

/* Floating-point exception handling.  */

/* Save the current environment in the object pointed to by ENVP, clear
   exception flags and install a non-stop mode (if available) for all
   exceptions.  */
extern int feholdexcept (fenv_t *__envp) __THROW;

/* Establish the floating-point environment represented by the object
   pointed to by ENVP.  */
extern int fesetenv (__const fenv_t *__envp) __THROW;

__END_DECLS

#endif /* fenv.h */
