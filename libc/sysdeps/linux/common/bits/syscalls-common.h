/*
 * Common syscall type defines
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#ifndef _SYSCALLS_COMMON_H
#define _SYSCALLS_COMMON_H	1

#ifndef _SYSCALL_H
# error "Never use <bits/syscalls-common.h> directly; include <sys/syscall.h> instead."
#endif

#ifndef SYS_ify
# define SYS_ify(syscall_name) (__NR_##syscall_name)
#endif

#ifndef __ASSEMBLER__

#include <errno.h>

#ifndef INTERNAL_SYSCALL_DECL
# define INTERNAL_SYSCALL_DECL(err)         do { } while (0)
#endif
#ifndef INTERNAL_SYSCALL_ERROR_P
# define INTERNAL_SYSCALL_ERROR_P(val, err) ((unsigned long)val >= (unsigned long)(-4095))
#endif
#ifndef INTERNAL_SYSCALL_ERRNO
# define INTERNAL_SYSCALL_ERRNO(val, err)   (-(val))
#endif

/* Define a macro which expands into the inline wrapper code for a system call */
#ifndef INLINE_SYSCALL
# define INLINE_SYSCALL(name, nr, args...)				\
({									\
	INTERNAL_SYSCALL_DECL(err);					\
	long res = INTERNAL_SYSCALL(name, err, nr, args);		\
	if (unlikely(INTERNAL_SYSCALL_ERROR_P(res, err))) {		\
		__set_errno(INTERNAL_SYSCALL_ERRNO(res, err));		\
		res = -1L;						\
	}								\
	res;								\
})
#endif

#ifndef _syscall0

#define C_DECL_ARGS_0()			void
#define C_DECL_ARGS_1(t, v)		t v
#define C_DECL_ARGS_2(t, v, args...)	t v, C_DECL_ARGS_1(args)
#define C_DECL_ARGS_3(t, v, args...)	t v, C_DECL_ARGS_2(args)
#define C_DECL_ARGS_4(t, v, args...)	t v, C_DECL_ARGS_3(args)
#define C_DECL_ARGS_5(t, v, args...)	t v, C_DECL_ARGS_4(args)
#define C_DECL_ARGS_6(t, v, args...)	t v, C_DECL_ARGS_5(args)

#define C_ARGS_0()
#define C_ARGS_1(t, v)			v
#define C_ARGS_2(t, v, args...)		v, C_ARGS_1(args)
#define C_ARGS_3(t, v, args...)		v, C_ARGS_2(args)
#define C_ARGS_4(t, v, args...)		v, C_ARGS_3(args)
#define C_ARGS_5(t, v, args...)		v, C_ARGS_4(args)
#define C_ARGS_6(t, v, args...)		v, C_ARGS_5(args)

#define SYSCALL_FUNC(nargs, type, name, args...)			\
type name(C_DECL_ARGS_##nargs(args)) {					\
	return (type)INLINE_SYSCALL(name, nargs, C_ARGS_##nargs(args));	\
}

#define _syscall0(args...)		SYSCALL_FUNC(0, args)
#define _syscall1(args...)		SYSCALL_FUNC(1, args)
#define _syscall2(args...)		SYSCALL_FUNC(2, args)
#define _syscall3(args...)		SYSCALL_FUNC(3, args)
#define _syscall4(args...)		SYSCALL_FUNC(4, args)
#define _syscall5(args...)		SYSCALL_FUNC(5, args)
#define _syscall6(args...)		SYSCALL_FUNC(6, args)

#endif /* _syscall0 */

#endif /* __ASSEMBLER__ */

#endif
