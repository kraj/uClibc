/* Copyright (C) 2000, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.
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

#ifndef _LINUX_MIPS_MIPS32_SYSDEP_H
#define _LINUX_MIPS_MIPS32_SYSDEP_H 1

/* There is some commonality.  */
/* NPTL - start sysdeps/unix/mips32/sysdep.h  */

/* NPTL - start sysdeps/unix/mips/sysdep.h  */

#include <sgidefs.h>
/* NPTL - start sysdeps/unix/sysdep.h  */

/* NPTL - start sysdeps/generic/sysdep.h  */
/* Define a macro we can use to construct the asm name for a C symbol.  */
#ifdef	NO_UNDERSCORES
#ifdef	__STDC__
#define C_LABEL(name)		name##:
#else
#define C_LABEL(name)		name/**/:
#endif
#else
#ifdef	__STDC__
#define C_LABEL(name)		_##name##:
#else
#define C_LABEL(name)		_/**/name/**/:
#endif
#endif

#ifdef __ASSEMBLER__
/* Mark the end of function named SYM.  This is used on some platforms
   to generate correct debugging information.  */
#ifndef END
#define END(sym)
#endif

#ifndef JUMPTARGET
#define JUMPTARGET(sym)		sym
#endif

/* Makros to generate eh_frame unwind information.  */
# ifdef HAVE_ASM_CFI_DIRECTIVES
#  define cfi_startproc			.cfi_startproc
#  define cfi_endproc			.cfi_endproc
#  define cfi_def_cfa(reg, off)		.cfi_def_cfa reg, off
#  define cfi_def_cfa_register(reg)	.cfi_def_cfa_register reg
#  define cfi_def_cfa_offset(off)	.cfi_def_cfa_offset off
#  define cfi_adjust_cfa_offset(off)	.cfi_adjust_cfa_offset off
#  define cfi_offset(reg, off)		.cfi_offset reg, off
#  define cfi_rel_offset(reg, off)	.cfi_rel_offset reg, off
#  define cfi_register(r1, r2)		.cfi_register r1, r2
#  define cfi_return_column(reg)	.cfi_return_column reg
#  define cfi_restore(reg)		.cfi_restore reg
#  define cfi_undefined(reg)		.cfi_undefined reg
#  define cfi_remember_state		.cfi_remember_state
#  define cfi_restore_state		.cfi_restore_state
#  define cfi_window_save		.cfi_window_save
# else
#  define cfi_startproc
#  define cfi_endproc
#  define cfi_def_cfa(reg, off)
#  define cfi_def_cfa_register(reg)
#  define cfi_def_cfa_offset(off)
#  define cfi_adjust_cfa_offset(off)
#  define cfi_offset(reg, off)
#  define cfi_rel_offset(reg, off)
#  define cfi_register(r1, r2)
#  define cfi_return_column(reg)
#  define cfi_restore(reg)
#  define cfi_undefined(reg)
#  define cfi_remember_state
#  define cfi_restore_state
#  define cfi_window_save
# endif

#else /* ! ASSEMBLER */
# ifdef HAVE_ASM_CFI_DIRECTIVES
#  define CFI_STRINGIFY(Name) CFI_STRINGIFY2 (Name)
#  define CFI_STRINGIFY2(Name) #Name
#  define CFI_STARTPROC	".cfi_startproc"
#  define CFI_ENDPROC	".cfi_endproc"
#  define CFI_DEF_CFA(reg, off)	\
   ".cfi_def_cfa " CFI_STRINGIFY(reg) "," CFI_STRINGIFY(off)
#  define CFI_DEF_CFA_REGISTER(reg) \
   ".cfi_def_cfa_register " CFI_STRINGIFY(reg)
#  define CFI_DEF_CFA_OFFSET(off) \
   ".cfi_def_cfa_offset " CFI_STRINGIFY(off)
#  define CFI_ADJUST_CFA_OFFSET(off) \
   ".cfi_adjust_cfa_offset " CFI_STRINGIFY(off)
#  define CFI_OFFSET(reg, off) \
   ".cfi_offset " CFI_STRINGIFY(reg) "," CFI_STRINGIFY(off)
#  define CFI_REL_OFFSET(reg, off) \
   ".cfi_rel_offset " CFI_STRINGIFY(reg) "," CFI_STRINGIFY(off)
#  define CFI_REGISTER(r1, r2) \
   ".cfi_register " CFI_STRINGIFY(r1) "," CFI_STRINGIFY(r2)
#  define CFI_RETURN_COLUMN(reg) \
   ".cfi_return_column " CFI_STRINGIFY(reg)
#  define CFI_RESTORE(reg) \
   ".cfi_restore " CFI_STRINGIFY(reg)
#  define CFI_UNDEFINED(reg) \
   ".cfi_undefined " CFI_STRINGIFY(reg)
#  define CFI_REMEMBER_STATE \
   ".cfi_remember_state"
#  define CFI_RESTORE_STATE \
   ".cfi_restore_state"
#  define CFI_WINDOW_SAVE \
   ".cfi_window_save"
# else
#  define CFI_STARTPROC
#  define CFI_ENDPROC
#  define CFI_DEF_CFA(reg, off)
#  define CFI_DEF_CFA_REGISTER(reg)
#  define CFI_DEF_CFA_OFFSET(off)
#  define CFI_ADJUST_CFA_OFFSET(off)
#  define CFI_OFFSET(reg, off)
#  define CFI_REL_OFFSET(reg, off)
#  define CFI_REGISTER(r1, r2)
#  define CFI_RETURN_COLUMN(reg)
#  define CFI_RESTORE(reg)
#  define CFI_UNDEFINED(reg)
#  define CFI_REMEMBER_STATE
#  define CFI_RESTORE_STATE
#  define CFI_WINDOW_SAVE
# endif

#endif /* __ASSEMBLER__ */
/* NPTL - end sysdeps/generic/sysdep.h  */

#include <sys/syscall.h>
#define	HAVE_SYSCALLS

#if 0
/* Note that using a `PASTE' macro loses.  */
#ifdef	__STDC__
#define	SYSCALL__(name, args)	PSEUDO (__##name, name, args)
#else
#define	SYSCALL__(name, args)	PSEUDO (__/**/name, name, args)
#endif
#define	SYSCALL(name, args)	PSEUDO (name, name, args)
#endif

/* Machine-dependent sysdep.h files are expected to define the macro
   PSEUDO (function_name, syscall_name) to emit assembly code to define the
   C-callable function FUNCTION_NAME to do system call SYSCALL_NAME.
   r0 and r1 are the system call outputs.  MOVE(x, y) should be defined as
   an instruction such that "MOVE(r1, r0)" works.  ret should be defined
   as the return instruction.  */

#if 0
/*
 * NPTL - these are defined in 'include/bits/syscalls.h' and
 *             'include/bits/sysnum.h'.
 */
#ifdef __STDC__
#define SYS_ify(syscall_name) SYS_##syscall_name
#else
#define SYS_ify(syscall_name) SYS_/**/syscall_name
#endif
#endif

/* Terminate a system call named SYM.  This is used on some platforms
   to generate correct debugging information.  */
#ifndef PSEUDO_END
#define PSEUDO_END(sym)
#endif
#ifndef PSEUDO_END_NOERRNO
#define PSEUDO_END_NOERRNO(sym)	PSEUDO_END(sym)
#endif
#ifndef PSEUDO_END_ERRVAL
#define PSEUDO_END_ERRVAL(sym)	PSEUDO_END(sym)
#endif

/* Wrappers around system calls should normally inline the system call code.
   But sometimes it is not possible or implemented and we use this code.  */
#define INLINE_SYSCALL(name, nr, args...) __syscall_##name (args)
/* NPTL - end sysdeps/unix/sysdep.h  */

#ifdef __ASSEMBLER__

#include <sys/regdef.h>

#define ENTRY(name) \
  .globl name;								      \
  .align 2;								      \
  .ent name,0;								      \
  name##:

#undef END
#define	END(function)                                   \
		.end	function;		        \
		.size	function,.-function

#define ret	j ra ; nop

#undef PSEUDO_END
#define PSEUDO_END(sym) .end sym; .size sym,.-sym

#define PSEUDO_NOERRNO(name, syscall_name, args)	\
  .align 2;						\
  ENTRY(name)						\
  .set noreorder;					\
  li v0, SYS_ify(syscall_name);				\
  syscall

#undef PSEUDO_END_NOERRNO
#define PSEUDO_END_NOERRNO(sym) .end sym; .size sym,.-sym

#define ret_NOERRNO ret

#define PSEUDO_ERRVAL(name, syscall_name, args)	\
  .align 2;						\
  ENTRY(name)						\
  .set noreorder;					\
  li v0, SYS_ify(syscall_name);				\
  syscall

#undef PSEUDO_END_ERRVAL
#define PSEUDO_END_ERRVAL(sym) .end sym; .size sym,.-sym

#define ret_ERRVAL ret

#define r0	v0
#define r1	v1
/* The mips move insn is d,s.  */
#define MOVE(x,y)	move y , x

#if _MIPS_SIM == _ABIO32
# define L(label) $L ## label
#else
# define L(label) .L ## label
#endif

#endif
/* NPTL - end sysdeps/unix/mips/sysdep.h  */

/* Note that while it's better structurally, going back to call __syscall_error
   can make things confusing if you're debugging---it looks like it's jumping
   backwards into the previous fn.  */
#ifdef __PIC__
#define PSEUDO(name, syscall_name, args) \
  .align 2;								      \
  99: la t9,__syscall_error;						      \
  jr t9;								      \
  ENTRY(name)								      \
  .set noreorder;							      \
  .cpload t9;								      \
  li v0, SYS_ify(syscall_name);						      \
  syscall;								      \
  .set reorder;								      \
  bne a3, zero, 99b;							      \
L(syse1):
#else
#define PSEUDO(name, syscall_name, args) \
  .set noreorder;							      \
  .align 2;								      \
  99: j __syscall_error;						      \
  nop;									      \
  ENTRY(name)								      \
  .set noreorder;							      \
  li v0, SYS_ify(syscall_name);						      \
  syscall;								      \
  .set reorder;								      \
  bne a3, zero, 99b;							      \
L(syse1):
#endif
/* NPTL - end sysdeps/unix/mips32/sysdep.h  */

/* For Linux we can use the system call table in the header file
	/usr/include/asm/unistd.h
   of the kernel.  But these symbols do not follow the SYS_* syntax
   so we have to redefine the `SYS_ify' macro here.  */
#undef SYS_ify
#ifdef __STDC__
# define SYS_ify(syscall_name)	__NR_##syscall_name
#else
# define SYS_ify(syscall_name)	__NR_/**/syscall_name
#endif

#ifdef __ASSEMBLER__

/* We don't want the label for the error handler to be visible in the symbol
   table when we define it here.  */
#ifdef __PIC__
# define SYSCALL_ERROR_LABEL 99b
#endif

#else   /* ! __ASSEMBLER__ */

/* Define a macro which expands into the inline wrapper code for a system
   call.  */
#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...)                               \
  ({ INTERNAL_SYSCALL_DECL(err);					\
     long result_var = INTERNAL_SYSCALL (name, err, nr, args);		\
     if ( INTERNAL_SYSCALL_ERROR_P (result_var, err) )			\
       {								\
	 result_var = -1L;						\
       }								\
     result_var; })

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

#endif /* __ASSEMBLER__ */

#endif /* linux/mips/mips32/sysdep.h */
