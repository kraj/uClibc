/*
 * Stolen from glibc-2.2.2 by Eddie C. Dost <ecd@atecom.com>
 */

	.text
	.globl	_dl_linux_resolve
	.type	_dl_linux_resolve, @function
	.balign	16
_dl_linux_resolve:
	mov.l	r3, @-r15
	mov.l	r4, @-r15
	mov.l	r5, @-r15
	mov.l	r6, @-r15
	mov.l	r7, @-r15
	mov.l	r12, @-r15
	movt	r3		! Save T flag
	mov.l	r3, @-r15

#ifdef HAVE_FPU
	sts.l	fpscr, @-r15
	mov	#8,r3
	swap.w	r3, r3
	lds	r3, fpscr
	fmov.s	fr11, @-r15
	fmov.s	fr10, @-r15
	fmov.s	fr9, @-r15
	fmov.s	fr8, @-r15
	fmov.s	fr7, @-r15
	fmov.s	fr6, @-r15
	fmov.s	fr5, @-r15
	fmov.s	fr4, @-r15
#endif
	sts.l	pr, @-r15

	mov	r2, r4		! link map address

	mov.l	3f, r0
	jsr	@r0		! Call resolver
	 mov	r1, r5		! Reloc offset

	lds.l	@r15+, pr	! Get register content back

#ifdef HAVE_FPU
	fmov.s	@r15+, fr4
	fmov.s	@r15+, fr5
	fmov.s	@r15+, fr6
	fmov.s	@r15+, fr7
	fmov.s	@r15+, fr8
	fmov.s	@r15+, fr9
	fmov.s	@r15+, fr10
	fmov.s	@r15+, fr11
	lds.l	@r15+, fpscr
#endif

	mov.l	@r15+, r3
	shal	r3		! Load T flag
	mov.l	@r15+, r12
	mov.l	@r15+, r7
	mov.l	@r15+, r6
	mov.l	@r15+, r5
	mov.l	@r15+, r4
	jmp	@r0		! Jump to function address
	 mov.l	@r15+, r3

	.balign	4
3:
	.long	_dl_linux_resolver
	.size	_dl_linux_resolve, . - _dl_linux_resolve

