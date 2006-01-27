# setjmp.S atp. Sept. 2001
# save regs and info needed for a longjmp

.globl __sigsetjmp
.align 4
__sigsetjmp:
	.word	0x0000		# we look after reg saving here - this must
				# match longjmp
	movl	0x4(%ap), %r0	# our scratch reg
	# kenn would probably use movq here. :-)
	movl	%r1, 0x14(%r0)	# save regs
	movl	%r2, 0x18(%r0)
	movl	%r3, 0x1c(%r0)
	movl	%r4, 0x20(%r0)
	movl	%r5, 0x24(%r0)
	movl	%r6, 0x28(%r0)
	movl	%r7, 0x2c(%r0)
	movl	%r8, 0x30(%r0)
	movl	%r9, 0x34(%r0)
	movl	%r10, 0x38(%r0)
	movl	%r11, 0x3c(%r0)
	# now save our call frame
	movl	(%fp), (%r0)		# condition handler (for VMS emulation)
	movl	0x4(%fp), 0x4(%r0)	# psw
	movl	0x8(%fp), 0x8(%r0)	# ap
	movl	0xc(%fp), 0xc(%r0)	# fp
	movl	0x10(%fp), 0x10(%r0)	# pc
	# call the sigjmp save routine
	pushl	8(%ap)
	pushl	%r0
	calls	$2, __sigjmp_save
	# done
	ret

