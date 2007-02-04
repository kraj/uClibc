/* Copyright (C) 2004 Free Software Foundation, Inc.
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

#ifndef _FENV_H
# error "Never use <bits/fenv.h> directly; include <fenv.h> instead."
#endif


/* Define bits representing the exception.  We use the bit positions of
   the appropriate bits in the SPEFSCR...  */
enum
  {
    FE_INEXACT = 1 << (63 - 42),
#define FE_INEXACT	FE_INEXACT
    FE_INVALID = 1 << (63 - 43),
#define FE_INVALID	FE_INVALID
    FE_DIVBYZERO = 1 << (63 - 44),
#define FE_DIVBYZERO	FE_DIVBYZERO
    FE_UNDERFLOW = 1 << (63 - 45),
#define FE_UNDERFLOW	FE_UNDERFLOW
    FE_OVERFLOW = 1 << (63 - 46)
#define FE_OVERFLOW	FE_OVERFLOW
  };

#define FE_ALL_EXCEPT \
	(FE_INEXACT | FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID)

/* The E500 support all of the four defined rounding modes.  We use
   the bit pattern in the SPEFSCR as the values for the appropriate
   macros.  */
enum
  {
    FE_TONEAREST = 0,
#define FE_TONEAREST	FE_TONEAREST
    FE_TOWARDZERO = 1,
#define FE_TOWARDZERO	FE_TOWARDZERO
    FE_UPWARD = 2,
#define FE_UPWARD	FE_UPWARD
    FE_DOWNWARD = 3
#define FE_DOWNWARD	FE_DOWNWARD
  };

/* Type representing exception flags.  */
typedef unsigned int fexcept_t;

typedef double fenv_t;

/* If the default argument is used we use this value.  */
extern const fenv_t __fe_dfl_env;
#define FE_DFL_ENV	(&__fe_dfl_env)

#ifdef __USE_GNU
/* Floating-point environment where all exceptions are enabled.  Note that
   this is not sufficient to give you SIGFPE.  */
extern const fenv_t __fe_enabled_env;
# define FE_ENABLED_ENV	(&__fe_enabled_env)

/* Floating-point environment with all exceptions enabled.  Note that
   just evaluating this value will set the processor into 'FPU
   exceptions imprecise recoverable' mode, which may cause a significant
   performance penalty (but have no other visible effect).  */
extern const fenv_t *__fe_nomask_env (void);
# define FE_NOMASK_ENV	(__fe_nomask_env ())
#endif
