/* Copyright (C) 1991, 1992 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include <sys/syscall.h>
#include <sys/regdef.h>
#include <features.h>

/* Not that using a `PASTE' macro loses.  */
#ifdef	__STDC__

#define SYSCALL_WEAK_ALIAS(alias,orig) \
	.weak alias; \
	alias=__libc_##orig

/* Use the regular ELF conventions about underscores, and provide the
   weak symbol, as required */
#define	SYSCALL__(name,args)	PSEUDO (__libc_##name, name, args) \
.weak __##name; \
.weak name;  \
  __##name = __libc_##name; \
  name = __libc_##name; \
.type __libc_##name,@function; \
.type name,@function; \
.type __##name,@function; \
.L__libc_##name##end: .size __libc_##name,.L__libc_##name##end - __libc_##name

#define	SYSCALL(name,args)	PSEUDO (__libc_##name, name, args) \
.weak name; \
  name = __libc_##name; \
.type __libc_##name,@function; \
.type name,@function; \
.L__libc_##name##end: .size __libc_##name,.L__libc_##name##end - __libc_##name

#else  /* __STDC__ */

#define	SYSCALL__(name,args)	PSEUDO (__/**/name, name, args)
#define	SYSCALL(name,args)	PSEUDO (name, name, args)

#endif /* __STDC__ */

#ifndef __ASSEMBLER__
/* Define a macro which expands into the inline wrapper code for a system
   call.  */
#undef INLINE_SYSCALL
#if 0
#define INLINE_SYSCALL(name, nr, args...)                               \
  ({ INTERNAL_SYSCALL_DECL(err);					\
     long result_var = INTERNAL_SYSCALL (name, err, nr, args);		\
     if ( INTERNAL_SYSCALL_ERROR_P (result_var, err) )			\
       {								\
	 __set_errno (INTERNAL_SYSCALL_ERRNO (result_var, err));	\
	 result_var = -1L;						\
       }								\
     result_var; })
#else
#define INLINE_SYSCALL(name, nr, args...)                               \
  ({ INTERNAL_SYSCALL_DECL(err);					\
     long result_var = INTERNAL_SYSCALL (name, err, nr, args);		\
     if ( INTERNAL_SYSCALL_ERROR_P (result_var, err) )			\
       {								\
	 result_var = __syscall_error((int) result_var);		\
       }								\
     result_var; })
#endif

#undef INTERNAL_SYSCALL_DECL
#define INTERNAL_SYSCALL_DECL(err) long err

#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val, err)   ((long) (err))

#undef INTERNAL_SYSCALL_ERRNO
#define INTERNAL_SYSCALL_ERRNO(val, err)     (val)

#undef INTERNAL_SYSCALL
#define INTERNAL_SYSCALL(name, err, nr, args...) \
	internal_syscall##nr (, "li\t$2, %2\t\t\t# " #name "\n\t",	\
			      "i" (SYS_ify (name)), err, args)

#undef INTERNAL_SYSCALL_NCS
#define INTERNAL_SYSCALL_NCS(number, err, nr, args...) \
	internal_syscall##nr (= number, , "r" (__v0), err, args)

#define internal_syscall0(ncs_init, cs_init, input, err, dummy...)	\
({									\
	long _sys_result;						\
									\
	{								\
	register long __v0 asm("$2") ncs_init;				\
	register long __a3 asm("$7");					\
	__asm__ volatile (						\
	".set\tnoreorder\n\t"						\
	cs_init								\
	"syscall\n\t"							\
	".set reorder"							\
	: "=r" (__v0), "=r" (__a3)					\
	: input								\
	: __SYSCALL_CLOBBERS);						\
	err = __a3;							\
	_sys_result = __v0;						\
	}								\
	_sys_result;							\
})

#define internal_syscall1(ncs_init, cs_init, input, err, arg1)		\
({									\
	long _sys_result;						\
									\
	{								\
	register long __v0 asm("$2") ncs_init;				\
	register long __a0 asm("$4") = (long) arg1;			\
	register long __a3 asm("$7");					\
	__asm__ volatile (						\
	".set\tnoreorder\n\t"						\
	cs_init								\
	"syscall\n\t"							\
	".set reorder"							\
	: "=r" (__v0), "=r" (__a3)					\
	: input, "r" (__a0)						\
	: __SYSCALL_CLOBBERS);						\
	err = __a3;							\
	_sys_result = __v0;						\
	}								\
	_sys_result;							\
})

#define internal_syscall2(ncs_init, cs_init, input, err, arg1, arg2)	\
({									\
	long _sys_result;						\
									\
	{								\
	register long __v0 asm("$2") ncs_init;				\
	register long __a0 asm("$4") = (long) arg1;			\
	register long __a1 asm("$5") = (long) arg2;			\
	register long __a3 asm("$7");					\
	__asm__ volatile (						\
	".set\tnoreorder\n\t"						\
	cs_init								\
	"syscall\n\t"							\
	".set\treorder"						\
	: "=r" (__v0), "=r" (__a3)					\
	: input, "r" (__a0), "r" (__a1)					\
	: __SYSCALL_CLOBBERS);						\
	err = __a3;							\
	_sys_result = __v0;						\
	}								\
	_sys_result;							\
})

#define internal_syscall3(ncs_init, cs_init, input, err, arg1, arg2, arg3)\
({									\
	long _sys_result;						\
									\
	{								\
	register long __v0 asm("$2") ncs_init;				\
	register long __a0 asm("$4") = (long) arg1;			\
	register long __a1 asm("$5") = (long) arg2;			\
	register long __a2 asm("$6") = (long) arg3;			\
	register long __a3 asm("$7");					\
	__asm__ volatile (						\
	".set\tnoreorder\n\t"						\
	cs_init								\
	"syscall\n\t"							\
	".set\treorder"						\
	: "=r" (__v0), "=r" (__a3)					\
	: input, "r" (__a0), "r" (__a1), "r" (__a2)			\
	: __SYSCALL_CLOBBERS);						\
	err = __a3;							\
	_sys_result = __v0;						\
	}								\
	_sys_result;							\
})

#define internal_syscall4(ncs_init, cs_init, input, err, arg1, arg2, arg3, arg4)\
({									\
	long _sys_result;						\
									\
	{								\
	register long __v0 asm("$2") ncs_init;				\
	register long __a0 asm("$4") = (long) arg1;			\
	register long __a1 asm("$5") = (long) arg2;			\
	register long __a2 asm("$6") = (long) arg3;			\
	register long __a3 asm("$7") = (long) arg4;			\
	__asm__ volatile (						\
	".set\tnoreorder\n\t"						\
	cs_init								\
	"syscall\n\t"							\
	".set\treorder"						\
	: "=r" (__v0), "+r" (__a3)					\
	: input, "r" (__a0), "r" (__a1), "r" (__a2)			\
	: __SYSCALL_CLOBBERS);						\
	err = __a3;							\
	_sys_result = __v0;						\
	}								\
	_sys_result;							\
})

/* We need to use a frame pointer for the functions in which we
   adjust $sp around the syscall, or debug information and unwind
   information will be $sp relative and thus wrong during the syscall.  As
   of GCC 3.4.3, this is sufficient.  */
#define FORCE_FRAME_POINTER alloca (4)

#define internal_syscall5(ncs_init, cs_init, input, err, arg1, arg2, arg3, arg4, arg5)\
({									\
	long _sys_result;						\
									\
	FORCE_FRAME_POINTER;						\
	{								\
	register long __v0 asm("$2") ncs_init;				\
	register long __a0 asm("$4") = (long) arg1;			\
	register long __a1 asm("$5") = (long) arg2;			\
	register long __a2 asm("$6") = (long) arg3;			\
	register long __a3 asm("$7") = (long) arg4;			\
	__asm__ volatile (						\
	".set\tnoreorder\n\t"						\
	"subu\t$29, 32\n\t"						\
	"sw\t%6, 16($29)\n\t"						\
	cs_init								\
	"syscall\n\t"							\
	"addiu\t$29, 32\n\t"						\
	".set\treorder"						\
	: "=r" (__v0), "+r" (__a3)					\
	: input, "r" (__a0), "r" (__a1), "r" (__a2),			\
	  "r" ((long)arg5)						\
	: __SYSCALL_CLOBBERS);						\
	err = __a3;							\
	_sys_result = __v0;						\
	}								\
	_sys_result;							\
})

#define internal_syscall6(ncs_init, cs_init, input, err, arg1, arg2, arg3, arg4, arg5, arg6)\
({									\
	long _sys_result;						\
									\
	FORCE_FRAME_POINTER;						\
	{								\
	register long __v0 asm("$2") ncs_init;				\
	register long __a0 asm("$4") = (long) arg1;			\
	register long __a1 asm("$5") = (long) arg2;			\
	register long __a2 asm("$6") = (long) arg3;			\
	register long __a3 asm("$7") = (long) arg4;			\
	__asm__ volatile (						\
	".set\tnoreorder\n\t"						\
	"subu\t$29, 32\n\t"						\
	"sw\t%6, 16($29)\n\t"						\
	"sw\t%7, 20($29)\n\t"						\
	cs_init								\
	"syscall\n\t"							\
	"addiu\t$29, 32\n\t"						\
	".set\treorder"						\
	: "=r" (__v0), "+r" (__a3)					\
	: input, "r" (__a0), "r" (__a1), "r" (__a2),			\
	  "r" ((long)arg5), "r" ((long)arg6)				\
	: __SYSCALL_CLOBBERS);						\
	err = __a3;							\
	_sys_result = __v0;						\
	}								\
	_sys_result;							\
})

#define internal_syscall7(ncs_init, cs_init, input, err, arg1, arg2, arg3, arg4, arg5, arg6, arg7)\
({									\
	long _sys_result;						\
									\
	FORCE_FRAME_POINTER;						\
	{								\
	register long __v0 asm("$2") ncs_init;				\
	register long __a0 asm("$4") = (long) arg1;			\
	register long __a1 asm("$5") = (long) arg2;			\
	register long __a2 asm("$6") = (long) arg3;			\
	register long __a3 asm("$7") = (long) arg4;			\
	__asm__ volatile (						\
	".set\tnoreorder\n\t"						\
	"subu\t$29, 32\n\t"						\
	"sw\t%6, 16($29)\n\t"						\
	"sw\t%7, 20($29)\n\t"						\
	"sw\t%8, 24($29)\n\t"						\
	cs_init								\
	"syscall\n\t"							\
	"addiu\t$29, 32\n\t"						\
	".set\treorder"						\
	: "=r" (__v0), "+r" (__a3)					\
	: input, "r" (__a0), "r" (__a1), "r" (__a2),			\
	  "r" ((long)arg5), "r" ((long)arg6), "r" ((long)arg7)		\
	: __SYSCALL_CLOBBERS);						\
	err = __a3;							\
	_sys_result = __v0;						\
	}								\
	_sys_result;							\
})

#define __SYSCALL_CLOBBERS "$1", "$3", "$8", "$9", "$10", "$11", "$12", "$13", \
	"$14", "$15", "$24", "$25", "memory"
#else
#define ENTRY(name)							\
	.globl	name;							\
	.align	2;							\
	.ent	name,0;							\
	name##:
#endif

#if _MIPS_SIM == _ABIO32
# define L(label) $L ## label
#else
# define L(label) .L ## label
#endif
