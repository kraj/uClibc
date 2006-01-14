/* vi: set sw=4 ts=4: */
/* Syscalls for uClibc
 *
 * Copyright (C) 2000 by Lineo, inc. and Erik Andersen
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 */

#include <errno.h>
#include <unistd.h>
#include <features.h>
#include <sys/types.h>
#include <sys/syscall.h>

#ifdef __NR_create_module

#if defined(__i386__) || defined(__m68k__) || defined(__arm__) || defined(__thumb__) || defined(__cris__) || defined(__i960__)
# define __NR___create_module  __NR_create_module
# ifdef __STR_NR_create_module
#  define __STR_NR___create_module  __STR_NR_create_module
# endif
static inline _syscall2(long, __create_module, const char *, name, size_t, size);
/* By checking the value of errno, we know if we have been fooled 
 * by the syscall2 macro making a very high address look like a 
 * negative, so we we fix it up here.  */
unsigned long create_module(const char *name, size_t size)
{
	long ret = __create_module(name, size);

	/* Jump through hoops to fixup error return codes */
	if (ret == -1 && errno > 125) {
		ret = -errno;
		__set_errno(0);
	}
	return ret;
}
#elif defined(__alpha__)
# define __NR___create_module  __NR_create_module
/* Alpha doesn't have the same problem, exactly, but a bug in older
   kernels fails to clear the error flag.  Clear it here explicitly.  */
static inline _syscall4(unsigned long, __create_module, const char *, name,
			size_t, size, size_t, dummy, size_t, err);
unsigned long create_module(const char *name, size_t size)
{
  return __create_module(name, size, 0, 0);
}
#else
/* Sparc, MIPS, etc don't mistake return values for errors. */ 
_syscall2(unsigned long, create_module, const char *, name, size_t, size);
#endif

#else /* !__NR_create_module */
caddr_t create_module(const char *name attribute_unused, size_t size attribute_unused)
{
	__set_errno(ENOSYS);
	return (caddr_t)-1;
}
#endif
