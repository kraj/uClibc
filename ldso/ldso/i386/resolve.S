#if 0
#include <sysdep.h>
#endif
/*
 * These are various helper routines that are needed to run an ELF image.
 */
#ifndef ALIGN
#define ALIGN 4
#endif

#ifndef NO_UNDERSCORE
#define RUN _linux_run
#define RESOLVE __dl_linux_resolve
#define EXIT __interpreter_exit
#define RESOLVER __dl_linux_resolver
#define INIT ___loader_bootstrap
#else
#define RUN linux_run
#define RESOLVE _dl_linux_resolve
#define RESOLVER _dl_linux_resolver
#define EXIT _interpreter_exit
#define INIT __loader_bootstrap
#endif

.text
.align ALIGN
	.align 16

.globl RESOLVE
	.type	RESOLVE,@function
RESOLVE:
	pusha				/* preserve all regs */
	lea	0x20(%esp),%eax		/* eax = tpnt and reloc_entry params */
	pushl	4(%eax)			/* push copy of reloc_entry param */
	pushl	(%eax)			/* push copy of tpnt param */
	pushl	%eax			/* _dl_linux_resolver expects a dummy
					 * param - this could be removed */
#ifdef __PIC__
	call	.L24
.L24:
	popl	%ebx
	addl	$_GLOBAL_OFFSET_TABLE_+[.-.L24],%ebx
	movl RESOLVER@GOT(%ebx),%ebx	/* eax = resolved func */
	call *%ebx
#else
	call RESOLVER
#endif
	movl	%eax,0x2C(%esp)		/* store func addr over original
					 * tpnt param */
	addl	$0xC,%esp		/* remove copy parameters */
	popa				/* restore regs */
	ret	$4			/* jump to func removing original
					 * reloc_entry param from stack */
.LFE2:
	.size RESOLVE,.LFE2-RESOLVE
