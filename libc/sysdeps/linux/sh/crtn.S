	.file	"crtn.S"

	.section .init
	mov	r14,r15
	lds.l	@r15+,pr
	rts	
	mov.l	@r15+,r14
.Lfe1:
	.size	_init,.Lfe1-_init

	.section .fini
	mov	r14,r15
	lds.l	@r15+,pr
	rts	
	mov.l	@r15+,r14
.Lfe2:
	.size	_fini,.Lfe2-_fini
