	.file	"crti.S"

	.section .init
	.align	2
	.global	_init
	.hidden _init
	.type	_init,@function
_init:
	mov.l	r14,@-r15
	sts.l	pr,@-r15
	mov	r15,r14

	.section .fini
	.align	2
	.global	_fini
	.hidden _fini
	.type	_fini,@function
_fini:
	mov.l	r14,@-r15
	sts.l	pr,@-r15
	mov	r15,r14

