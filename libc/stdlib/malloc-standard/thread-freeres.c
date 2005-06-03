/* Free resources stored in thread-local variables on thread exit.
   Copyright (C) 2003 Free Software Foundation, Inc.
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

#include <stdlib.h>
#include <libc-symbols.h>

/* Define a hook variable called NAME.  Functions put on this hook take
   arguments described by PROTO.  Use `text_set_element (NAME, FUNCTION)'
   from gnu-stabs.h to add a function to the hook.  */

# define DEFINE_HOOK(NAME, PROTO)               \
  typedef void __##NAME##_hook_function_t PROTO; \
  symbol_set_define (NAME)

/* Run all the functions hooked on the set called NAME.
   Each function is called like this: `function ARGS'.  */

# define RUN_HOOK(NAME, ARGS)						      \
do {									      \
  void *const *__unbounded ptr;						      \
  for (ptr = (void *const *) symbol_set_first_element (NAME);		      \
       ! symbol_set_end_p (NAME, ptr); ++ptr)				      \
    (*(__##NAME##_hook_function_t *) *ptr) ARGS;			      \
} while (0)


DEFINE_HOOK (__libc_thread_subfreeres, (void));

void __attribute__ ((section ("__libc_thread_freeres_fn")))
__libc_thread_freeres (void)
{
  RUN_HOOK (__libc_thread_subfreeres, ());
}
