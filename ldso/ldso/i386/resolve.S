/*
 * These are various helper routines that are needed to run an ELF image.
 */

.text
.align 4

.globl _dl_linux_resolve
.type	_dl_linux_resolve,@function

_dl_linux_resolve:
	pusha				/* preserve all regs */
	lea	0x20(%esp),%eax		/* eax = tpnt and reloc_entry params */
	pushl	4(%eax)			/* push copy of reloc_entry param */
	pushl	(%eax)			/* push copy of tpnt param */
					 
#ifdef __PIC__
	call	.L24
.L24:
	popl	%ebx
	addl	$_GLOBAL_OFFSET_TABLE_+[.-.L24],%ebx
	movl _dl_linux_resolver@GOT(%ebx),%ebx	/* eax = resolved func */
	call *%ebx
#else
	call _dl_linux_resolver
#endif
	movl	%eax,0x28(%esp)		/* store func addr over original
					 * tpnt param */
	addl	$0x8,%esp		/* remove copy parameters */
	popa				/* restore regs */
	ret	$4			/* jump to func removing original
					 * reloc_entry param from stack */
.LFE2:
	.size _dl_linux_resolve,.LFE2-_dl_linux_resolve
