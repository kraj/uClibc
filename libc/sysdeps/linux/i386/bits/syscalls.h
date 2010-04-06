#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H

#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

/*
 * Some of the sneaky macros in the code were taken from
 * glibc-2.2.5/sysdeps/unix/sysv/linux/i386/sysdep.h
 */

#ifndef __ASSEMBLER__

#include <errno.h>

/* We need some help from the assembler to generate optimal code.
 * We define some macros here which later will be used.  */

#if defined __SUPPORT_LD_DEBUG__ && defined __DOMULTI__
#error LD debugging and DOMULTI are incompatible
#endif

#ifdef __DOMULTI__
__asm__ (
	".L__X'%ebx = 1\n\t"
	".L__X'%ecx = 2\n\t"
	".L__X'%edx = 2\n\t"
	".L__X'%eax = 3\n\t"
	".L__X'%esi = 3\n\t"
	".L__X'%edi = 3\n\t"
	".L__X'%ebp = 3\n\t"
	".L__X'%esp = 3\n\t"

	".ifndef _BITS_SYSCALLS_ASM\n\t"
	".set _BITS_SYSCALLS_ASM,1\n\t"

	/* Loading param #1 (ebx) is done by loading it into
	 * another register, and then performing bpushl+bmovl,
	 * since we must preserve ebx */

	".macro bpushl name reg\n\t"
	".if 1 - \\name\n\t"    /* if reg!=ebx... */
	".if 2 - \\name\n\t"    /* if reg can't be clobbered... */
	"pushl %ebx\n\t"        /* save ebx on stack */
	".else\n\t"
	"xchgl \\reg, %ebx\n\t" /* else save ebx in reg, and load reg to ebx */
	".endif\n\t"
	".endif\n\t"
	".endm\n\t"

	".macro bmovl name reg\n\t"
	".if 1 - \\name\n\t"
	".if 2 - \\name\n\t"    /* if reg can't be clobbered... */
	"movl \\reg, %ebx\n\t"  /* load reg to ebx */
	".endif\n\t"
	".endif\n\t"
	".endm\n\t"

	".endif\n\t" /* _BITS_SYSCALLS_ASM */

	".macro bpopl name reg\n\t"
	".if 1 - \\name\n\t"
	".if 2 - \\name\n\t"    /* if reg can't be clobbered... */
	"popl %ebx\n\t"         /* restore ebx from stack */
	".else\n\t"
	"xchgl \\reg, %ebx\n\t" /* else restore ebx from reg */
	".endif\n\t"
	".endif\n\t"
	".endm\n\t"
);
#else
__asm__ (
	".L__X'%ebx = 1\n\t"
	".L__X'%ecx = 2\n\t"
	".L__X'%edx = 2\n\t"
	".L__X'%eax = 3\n\t"
	".L__X'%esi = 3\n\t"
	".L__X'%edi = 3\n\t"
	".L__X'%ebp = 3\n\t"
	".L__X'%esp = 3\n\t"

	".macro bpushl name reg\n\t"
	".if 1 - \\name\n\t"
	".if 2 - \\name\n\t"
	"pushl %ebx\n\t"
	".else\n\t"
	"xchgl \\reg, %ebx\n\t"
	".endif\n\t"
	".endif\n\t"
	".endm\n\t"

	".macro bmovl name reg\n\t"
	".if 1 - \\name\n\t"
	".if 2 - \\name\n\t"
	"movl \\reg, %ebx\n\t"
	".endif\n\t"
	".endif\n\t"
	".endm\n\t"

	".macro bpopl name reg\n\t"
	".if 1 - \\name\n\t"
	".if 2 - \\name\n\t"
	"popl %ebx\n\t"
	".else\n\t"
	"xchgl \\reg, %ebx\n\t"
	".endif\n\t"
	".endif\n\t"
	".endm\n\t"
);
#endif

#define INTERNAL_SYSCALL_NCS(name, err, nr, args...) \
({ \
	register unsigned int resultvar; \
	__asm__ __volatile__ ( \
		LOADARGS_##nr                                   \
		"movl	%1, %%eax\n\t"                          \
		"int	$0x80\n\t"                              \
		RESTOREARGS_##nr                                \
		: "=a" (resultvar)                              \
		: "i" (name) ASMFMT_##nr(args) : "memory", "cc" \
	); \
	(int) resultvar; \
})

#define LOADARGS_0
#define LOADARGS_1  "bpushl .L__X'%k2, %k2\n\t" "bmovl .L__X'%k2, %k2\n\t"
#define LOADARGS_2  LOADARGS_1
#define LOADARGS_3  LOADARGS_1
#define LOADARGS_4  LOADARGS_1
#define LOADARGS_5  LOADARGS_1
#define LOADARGS_6  LOADARGS_1 "push %%ebp\n\t" "movl %7, %%ebp\n\t"

#define RESTOREARGS_0
#define RESTOREARGS_1  "bpopl .L__X'%k2, %k2\n\t"
#define RESTOREARGS_2  RESTOREARGS_1
#define RESTOREARGS_3  RESTOREARGS_1
#define RESTOREARGS_4  RESTOREARGS_1
#define RESTOREARGS_5  RESTOREARGS_1
#define RESTOREARGS_6  "pop %%ebp\n\t" RESTOREARGS_1

#define ASMFMT_0()
#define ASMFMT_1(arg1) \
	, "acdSD" (arg1)
#define ASMFMT_2(arg1, arg2) \
	, "adSD" (arg1), "c" (arg2)
#define ASMFMT_3(arg1, arg2, arg3) \
	, "aSD" (arg1), "c" (arg2), "d" (arg3)
#define ASMFMT_4(arg1, arg2, arg3, arg4) \
	, "aD" (arg1), "c" (arg2), "d" (arg3), "S" (arg4)
#define ASMFMT_5(arg1, arg2, arg3, arg4, arg5) \
	, "a" (arg1), "c" (arg2), "d" (arg3), "S" (arg4), "D" (arg5)
#define ASMFMT_6(arg1, arg2, arg3, arg4, arg5, arg6) \
	, "a" (arg1), "c" (arg2), "d" (arg3), "S" (arg4), "D" (arg5), "m" (arg6)

#endif /* __ASSEMBLER__ */
#endif /* _BITS_SYSCALLS_H */
