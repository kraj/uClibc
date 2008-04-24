#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

/*
 * This includes the `__NR_<name>' syscall numbers taken from the
 * Linux kernel header files. It also defines the traditional
 * `SYS_<name>' macros for older programs.
 */
#include <bits/sysnum.h>

#ifndef __ASSEMBLER__

#include <errno.h>

#define SYS_ify(syscall_name) (__NR_##syscall_name)

#undef _syscall0
#define _syscall0(type,name)				\
	type name(void)					\
	{						\
		return (type)(INLINE_SYSCALL(name, 0));	\
	}

#undef _syscall1
#define _syscall1(type,name,type1,arg1)				\
	type name(type1 arg1)					\
	{							\
		return (type)(INLINE_SYSCALL(name, 1, arg1));	\
	}

#undef _syscall2
#define _syscall2(type,name,type1,arg1,type2,arg2)			\
	type name(type1 arg1, type2 arg2)				\
	{								\
		return (type)(INLINE_SYSCALL(name, 2, arg1, arg2));	\
	}

#undef _syscall3
#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3)		\
	type name(type1 arg1, type2 arg2, type3 arg3)			\
	{								\
		return (type)(INLINE_SYSCALL(name, 3, arg1,		\
					     arg2, arg3));		\
	}

#undef _syscall4
#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,		\
		  type4,arg4)						\
	type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4)	\
	{								\
		return (type)(INLINE_SYSCALL(name, 4, arg1, arg2,	\
					     arg3, arg4));		\
	}

#undef _syscall5
#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,		\
		  type4,arg4,type5,arg5)				\
	type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4,	\
		  type5 arg5)						\
	{								\
		return (type)(INLINE_SYSCALL(name, 5, arg1, arg2,	\
					     arg3, arg4, arg5));	\
	}

#undef _syscall6
#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,		\
		  type4,arg4,type5,arg5,type6,arg6)			\
	type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4,	\
		  type5 arg5, type6 arg6)				\
	{								\
		return (type)(INLINE_SYSCALL(name, 6, arg1, arg2, arg3,	\
					     arg4, arg5, arg6));	\
	}

#undef unlikely
#define unlikely(x) __builtin_expect((x), 0)

#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...)				\
	({								\
		unsigned _sys_result = INTERNAL_SYSCALL(name, , nr, args); \
		if (unlikely(INTERNAL_SYSCALL_ERROR_P(_sys_result, ))) { \
			__set_errno(INTERNAL_SYSCALL_ERRNO(_sys_result, )); \
			_sys_result = (unsigned int) -1;		\
		}							\
		(int) _sys_result;					\
	})

#undef INTERNAL_SYSCALL_DECL
#define INTERNAL_SYSCALL_DECL(err) do { } while(0)

#undef INTERNAL_SYSCALL
#define INTERNAL_SYSCALL(name, err, nr, args...)			\
	({								\
		register int _a1 asm ("r12");				\
		register int _scno asm("r8") = SYS_ify(name);		\
		LOAD_ARGS_##nr (args);					\
		asm volatile ("scall	/* syscall " #name " */"	\
			      : "=r" (_a1)				\
			      : "r"(_scno) ASM_ARGS_##nr		\
			      : "cc", "memory");			\
		_a1;							\
	})

#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val, err)		\
	((unsigned int)(val) >= 0xfffff001U)

#undef INTERNAL_SYSCALL_ERRNO
#define INTERNAL_SYSCALL_ERRNO(val, errr) (-(val))

#define LOAD_ARGS_0() do { } while(0)
#define ASM_ARGS_0
#define LOAD_ARGS_1(a1)					\
	_a1 = (int) (a1);				\
	LOAD_ARGS_0()
#define ASM_ARGS_1	ASM_ARGS_0, "r"(_a1)
#define LOAD_ARGS_2(a1, a2)				\
	register int _a2 asm("r11") = (int)(a2);	\
	LOAD_ARGS_1(a1)
#define ASM_ARGS_2	ASM_ARGS_1, "r"(_a2)
#define LOAD_ARGS_3(a1, a2, a3)				\
	register int _a3 asm("r10") = (int)(a3);	\
	LOAD_ARGS_2(a1, a2)
#define ASM_ARGS_3	ASM_ARGS_2, "r"(_a3)
#define LOAD_ARGS_4(a1, a2, a3, a4)			\
	register int _a4 asm("r9") = (int)(a4);		\
	LOAD_ARGS_3(a1, a2, a3)
#define ASM_ARGS_4	ASM_ARGS_3, "r"(_a4)
#define LOAD_ARGS_5(a1, a2, a3, a4, a5)			\
	register int _a5 asm("r5") = (int)(a5);		\
	LOAD_ARGS_4(a1, a2, a3, a4)
#define ASM_ARGS_5	ASM_ARGS_4, "r"(_a5)
#define LOAD_ARGS_6(a1, a2, a3, a4, a5, a6)		\
	register int _a6 asm("r3") = (int)(a6);		\
	LOAD_ARGS_5(a1, a2, a3, a4, a5)
#define ASM_ARGS_6	ASM_ARGS_5, "r"(_a6)

#endif /* __ASSEMBLER__ */
#endif /* _BITS_SYSCALLS_H */
